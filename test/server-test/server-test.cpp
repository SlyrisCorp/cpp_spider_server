//
// Created by doom on 18/09/17.
//

#include <gtest/gtest.h>
#include <Protocol/Messages.hpp>
#include <Protocol/CommandHandler.hpp>

TEST(Protocol, SerializationReplyCode)
{
    spi::proto::ReplyCode rep;

    rep.code = 123;

    spi::Buffer buf;
    rep.serialize(buf);
    ASSERT_EQ(buf.size(), spi::proto::ReplyCode::SerializedSize);

    spi::proto::ReplyCode result(buf);
    ASSERT_EQ(result.code, 123u);
}

TEST(Protocol, SerializationKeyEvent)
{
    spi::proto::KeyEvent ke;

    ke.code = spi::proto::KeyCode::M;
    ke.state = spi::proto::KeyState::Down;
    ke.timestamp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());

    spi::Buffer buf;
    ke.serialize(buf);
    ASSERT_EQ(buf.size(), spi::proto::KeyEvent::SerializedSize);

    spi::proto::KeyEvent result(buf);
    ASSERT_EQ(ke.code, result.code);
    ASSERT_EQ(ke.state, result.state);
    ASSERT_EQ(ke.timestamp.time_since_epoch().count(), result.timestamp.time_since_epoch().count());
}

TEST(Protocol, SerializationRawData)
{
    std::string data = "this is a test";

    spi::proto::RawData raw;

    raw.bytes.insert(raw.bytes.begin(), data.begin(), data.end());

    spi::Buffer buf;
    raw.serialize(buf);
    ASSERT_EQ(buf.size(), sizeof(uint32_t) + data.length());

    spi::proto::RawData result(buf);
    ASSERT_EQ(result.bytes.size(), raw.bytes.size());
    for (size_t i = 0; i < result.bytes.size(); ++i) {
        ASSERT_EQ(result.bytes[i], raw.bytes[i]);
    }

    std::string resultStr((char *)result.bytes.data(), result.bytes.size());
    ASSERT_EQ(data, resultStr);
}

TEST(Protocol, Hello)
{
    spi::proto::Hello h;

    h.macAddress.get();
    h.md5 = utils::MD5("this is a test");
    h.port = 12345;
    h.version = 1;

    spi::Buffer buf;
    h.serialize(buf);
    ASSERT_EQ(buf.size(), 26u);

    spi::proto::Hello result(buf);

    ASSERT_EQ(h.macAddress.raw(), result.macAddress.raw());
    ASSERT_EQ(h.md5.raw(), result.md5.raw());
    ASSERT_EQ(h.port, result.port);
    ASSERT_EQ(h.version, result.version);
}

TEST(Protocol, Empty)
{
    spi::Buffer empty;

    ASSERT_THROW(spi::proto::ReplyCode code(empty), spi::UnserializationError);
    ASSERT_THROW(spi::proto::RawData raw(empty), spi::UnserializationError);
    ASSERT_THROW(spi::proto::Hello hello(empty), spi::UnserializationError);
    ASSERT_THROW(spi::proto::KeyEvent ke(empty), spi::UnserializationError);
    ASSERT_THROW(spi::proto::MouseClick mc(empty), spi::UnserializationError);
    ASSERT_THROW(spi::proto::MouseMove mm(empty), spi::UnserializationError);
    ASSERT_THROW(spi::proto::ImageData id(empty), spi::UnserializationError);
}

TEST(ProtocolMessage, MessageIdentification)
{
    spi::CommandHandler cmdHandler;
    spi::Buffer buf{0x01, 0x01, 0x01, 0x01};

    ASSERT_EQ(cmdHandler.identifyMessage(buf), spi::proto::MessageType::Unknown);

    buf = cmdHandler.makeHeader(spi::proto::MessageType::Hello);
    ASSERT_EQ(cmdHandler.identifyMessage(buf), spi::proto::MessageType::Hello);
}