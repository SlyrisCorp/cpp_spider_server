//
// Created by doom on 28/09/17.
//

#ifndef SPIDER_SERVER_MESSAGES_HPP
#define SPIDER_SERVER_MESSAGES_HPP

#include <cstring>
#include <chrono>
#include <vector>
#include <algorithm>
#include <utils/Utils.hpp>
#include <utils/Endian.hpp>
#include <utils/MD5.hpp>
#include <Utils/ILoggable.hpp>
#include <net/MACAddress.hpp>
#include <Protocol/MessagesEnums.hpp>

namespace spi::proto
{
    struct ReplyCode : public ILoggable
    {
        uint32_t code;

        static constexpr const size_t SerializedSize = 4;

        ReplyCode() noexcept = default;

        ReplyCode(const Buffer &buff)
        {
            code = Serializer::unserializeInt(buff, 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeInt(out, code);
        }

        std::string stringify() const noexcept override
        {
            return "[ReplyCode] " + std::to_string(code);
        }
    };

    struct Bye : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        std::string stringify() const noexcept override
        {
            return "[Bye]";
        }
    };

    struct RawData : public ILoggable
    {
        std::vector<Byte> bytes;

        RawData() = default;

        RawData(const Buffer &buff)
        {
            bytes = Serializer::unserializeBuff(buff, 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + bytes.size() + 4);

            Serializer::serializeInt(out, static_cast<uint32_t>(bytes.size()));
            out.insert(out.end(), bytes.begin(), bytes.end());
        }

        std::string stringify() const noexcept override
        {
            return "[RawData] size: " + std::to_string(bytes.size());
        }
    };

    struct Hello : public ILoggable
    {
        ::net::MACAddress macAddress;
        utils::MD5 md5;
        uint16_t version;
        uint16_t port;

        static constexpr const size_t SerializedSize = 26;

        Hello() = default;

        Hello(const Buffer &buff)
        {
            auto macAddrBytes = Serializer::unserializeBytes(buff, 0, 6);
            ::net::MACAddress::RawMACAddress rawAddr;
            std::copy(macAddrBytes.begin(), macAddrBytes.end(), rawAddr.begin());
            macAddress.setRaw(rawAddr);
            version = Serializer::unserializeShort(buff, 6);

            auto md5Bytes = Serializer::unserializeBytes(buff, 8, 16);
            utils::MD5::RawMD5 raw;
            std::copy(md5Bytes.begin(), md5Bytes.end(), raw.begin());
            md5.setRaw(raw);
            port = Serializer::unserializeShort(buff, 24);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            std::vector<Byte> macAddrBytes(macAddress.raw().begin(), macAddress.raw().end());
            Serializer::serializeBytes(out, macAddrBytes);

            Serializer::serializeShort(out, version);

            std::vector<Byte> bytes(md5.raw().begin(), md5.raw().end());
            Serializer::serializeBytes(out, bytes);

            Serializer::serializeShort(out, port);
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[Hello] ";
            ss << "MAC: " << macAddress.toString() << ", ";
            ss << "version: " << version << ", ";
            ss << "MD5: " << md5.toString() << ", ";
            ss << "port: " << port;
            return ss.str();
        }
    };

    struct KeyEvent : public ILoggable
    {
        std::chrono::steady_clock::time_point timestamp;
        KeyCode code;
        KeyState state;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        KeyEvent() = default;

        KeyEvent(const Buffer &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            code = static_cast<KeyCode::EnumType>(Serializer::unserializeInt(buff, 8));
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, 12));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, static_cast<uint32_t>(code));
            Serializer::serializeInt(out, static_cast<uint32_t>(state));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[KeyEvent] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "code: " << code.toString() << ", ";
            ss << "state: " << state.toString();
            return ss.str();
        }
    };

    struct MouseClick : public ILoggable
    {
        std::chrono::steady_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;
        KeyState state;
        MouseButton button;

        static constexpr const size_t SerializedSize = 8 + 4 + 4 + 4 + 4;

        MouseClick() = default;

        MouseClick(const Buffer &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            x = Serializer::unserializeInt(buff, 8);
            y = Serializer::unserializeInt(buff, 12);
            state = static_cast<KeyState::EnumType>(Serializer::unserializeInt(buff, 16));
            button = static_cast<MouseButton::EnumType>(Serializer::unserializeInt(buff, 20));
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + SerializedSize);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, x);
            Serializer::serializeInt(out, y);
            Serializer::serializeInt(out, static_cast<uint32_t>(state));
            Serializer::serializeInt(out, static_cast<uint32_t>(button));
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[MouseClick] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "x: " << x << ", ";
            ss << "y: " << y << ", ";
            ss << "state: " << state.toString() << ", ";
            ss << "button: " << button.toString();
            return ss.str();
        }
    };

    struct MouseMove : public ILoggable
    {
        std::chrono::steady_clock::time_point timestamp;
        uint32_t x;
        uint32_t y;

        static constexpr const size_t SerializedSize = 8 + 4 + 4;

        MouseMove() = default;

        MouseMove(const Buffer &buff)
        {
            timestamp = Serializer::unserializeTimestamp(buff, 0);
            x = Serializer::unserializeInt(buff, 8);
            y = Serializer::unserializeInt(buff, 12);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(SerializedSize);

            Serializer::serializeTimestamp(out, timestamp);
            Serializer::serializeInt(out, x);
            Serializer::serializeInt(out, y);
        }

        std::string stringify() const noexcept override
        {
            std::stringstream ss;

            ss << "[MouseMove] ";
            ss << "timestamp: " << timestamp.time_since_epoch().count() << ", ";
            ss << "x: " << x << ", ";
            ss << "y: " << y << ", ";
            return ss.str();
        }
    };

    struct ImageData : public ILoggable
    {
        std::vector<Byte> bytes;

        ImageData(const Buffer &buff)
        {
            bytes = Serializer::unserializeBuff(buff, 0);
        }

        void serialize(Buffer &out) const noexcept override
        {
            out.reserve(out.size() + bytes.size() + sizeof(uint32_t));

            Serializer::serializeBuff(out, bytes);
        }

        std::string stringify() const noexcept override
        {
            return "[ImageData] size: " + std::to_string(bytes.size());
        }
    };

    struct Screenshot : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        std::string stringify() const noexcept override
        {
            return "[Screenshot] requested screenshot";
        }
    };

    struct StealthMode : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        std::string stringify() const noexcept override
        {
            return "[StealthMode] requested switch to stealth mode";
        }
    };

    struct ActiveMode : public ILoggable
    {
        static constexpr const size_t SerializedSize = 0;

        void serialize([[maybe_unused]] Buffer &out) const noexcept override
        {
        }

        std::string stringify() const noexcept override
        {
            return "[ActiveMode] requested switch to active mode";
        }
    };
}

#endif //SPIDER_SERVER_MESSAGES_HPP
