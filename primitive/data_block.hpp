#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <algorithm>

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
            b.bytes.push_back(v ? 1 : 0);
            return b;
        }

        static data_block from_string(const std::string& str)
        {
            data_block b;
            b.bytes.assign(str.begin(), str.end());
            return b;
        }

        int32_t as_int32() const
        {
            if (bytes.size() >= sizeof(int32_t))
            {
                int32_t v;
                std::memcpy(&v, bytes.data(), sizeof(v));
                return v;
            }
            return 0;
        }

        int64_t as_int64() const
        {
            if (bytes.size() >= sizeof(int64_t))
            {
                int64_t v;
                std::memcpy(&v, bytes.data(), sizeof(v));
                return v;
            }
            if (bytes.size() >= sizeof(int32_t))
            {
                int32_t v;
                std::memcpy(&v, bytes.data(), sizeof(v));
                return static_cast<int64_t>(v);
            }
            return 0;
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
