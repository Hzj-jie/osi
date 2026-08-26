#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "../../math/big_uint.hpp"
#include "../../math/big_udec.hpp"

namespace primitive
{
    struct data_block_prefix
    {
        constexpr static char int_type = 'i';
        constexpr static char long_type = 'l';
        constexpr static char double_type = 'd';
        constexpr static char boolean_type = 'b';
        constexpr static char array_type = 'a';
        constexpr static char c_escaped_string_type = 'E';
        constexpr static char string_type = 's';
    };

    class data_block
    {
    public:
        std::vector<uint8_t> bytes;

        data_block() = default;
        explicit data_block(std::vector<uint8_t> b) : bytes(std::move(b)) {}
        explicit data_block(int32_t v) : data_block(from_int32(v)) {}
        explicit data_block(uint32_t v) : data_block(from_int32(static_cast<int32_t>(v))) {}
        explicit data_block(int64_t v) : data_block(from_int64(v)) {}
        explicit data_block(uint64_t v) : data_block(from_int64(static_cast<int64_t>(v))) {}
        explicit data_block(bool v) : data_block(from_bool(v)) {}
        explicit data_block(const std::string& v) : data_block(from_string(v)) {}
        explicit data_block(const char* v) : data_block(from_string(v != nullptr ? std::string(v) : std::string())) {}

        size_t value_bytes_size() const { return bytes.size(); }

        std::string to_assembly_string() const
        {
            static const char hex_chars[] = "0123456789abcdef";
            std::string res = "a";
            res.reserve(1 + bytes.size() * 2);
            for (uint8_t b : bytes)
            {
                res += hex_chars[(b >> 4) & 0x0F];
                res += hex_chars[b & 0x0F];
            }
            return res;
        }

        static data_block from_int32(int32_t v)
        {
            data_block b;
            b.bytes.resize(sizeof(v));
            std::memcpy(b.bytes.data(), &v, sizeof(v));
            return b;
        }

        static data_block from_int64(int64_t v)
        {
            data_block b;
            b.bytes.resize(sizeof(v));
            std::memcpy(b.bytes.data(), &v, sizeof(v));
            return b;
        }

        static data_block from_double(double v)
        {
            data_block b;
            b.bytes.resize(sizeof(v));
            std::memcpy(b.bytes.data(), &v, sizeof(v));
            return b;
        }

        static data_block from_bool(bool v)
        {
            data_block b;
            b.bytes.push_back(v ? 0xFF : 0x00);
            return b;
        }

        static data_block from_string(const std::string& str)
        {
            data_block b;
            b.bytes.assign(str.begin(), str.end());
            return b;
        }

        static data_block from_big_uint(const osi::math::big_uint& val)
        {
            return data_block(val.as_bytes());
        }

        static data_block from_big_udec(const osi::math::big_udec& val)
        {
            return data_block(val.as_bytes());
        }

        osi::math::big_uint as_big_uint() const
        {
            return osi::math::big_uint(bytes);
        }

        osi::math::big_udec as_big_udec() const
        {
            return osi::math::big_udec(bytes);
        }

        int32_t as_int32() const
        {
            if (bytes.empty()) return 0;
            if (bytes.size() >= sizeof(int32_t))
            {
                int32_t v;
                std::memcpy(&v, bytes.data(), sizeof(v));
                return v;
            }
            int32_t v = 0;
            for (size_t i = 0; i < bytes.size(); ++i)
            {
                v |= (static_cast<int32_t>(bytes[i]) << (i * 8));
            }
            return v;
        }

        int64_t as_int64() const
        {
            if (bytes.empty()) return 0;
            if (bytes.size() >= sizeof(int64_t))
            {
                int64_t v;
                std::memcpy(&v, bytes.data(), sizeof(v));
                return v;
            }
            int64_t v = 0;
            for (size_t i = 0; i < bytes.size(); ++i)
            {
                v |= (static_cast<int64_t>(bytes[i]) << (i * 8));
            }
            return v;
        }

        double as_double() const
        {
            if (bytes.size() >= sizeof(double))
            {
                double v;
                std::memcpy(&v, bytes.data(), sizeof(v));
                return v;
            }
            return 0.0;
        }

        bool as_bool() const
        {
            if (bytes.empty()) return false;
            for (uint8_t b : bytes) if (b != 0) return true;
            return false;
        }

        std::string as_string() const
        {
            return std::string(bytes.begin(), bytes.end());
        }

        bool operator==(const data_block& other) const
        {
            return bytes == other.bytes;
        }

        bool operator!=(const data_block& other) const
        {
            return bytes != other.bytes;
        }
    };
}
