#pragma once
#include <cstdint>
#include <stdexcept>
#include "../app_info/assert.hpp"

namespace primitive
{
    enum class ref_type : uint8_t
    {
        abs = 0,
        rel = 1,
        habs = 2,
        hrel = 3
    };

    class data_ref
    {
    public:
        constexpr static int64_t max_value = INT64_MAX >> 2;
        constexpr static int64_t rel_min_value = INT64_MIN >> 2;
        constexpr static int64_t abs_min_value = 0;

        ref_type type;
        int64_t offset;

        constexpr data_ref() : type(ref_type::abs), offset(0) {}
        constexpr data_ref(ref_type t, int64_t off) : type(t), offset(off) {}

        static bool rel(int64_t off, data_ref& out)
        {
            if (off > max_value || off < rel_min_value) return false;
            out.type = ref_type::rel;
            out.offset = off;
            return true;
        }

        static data_ref rel(int64_t off)
        {
            data_ref out;
            assert(rel(off, out));
            return out;
        }

        static bool hrel(int64_t off, data_ref& out)
        {
            if (off > max_value || off < rel_min_value) return false;
            out.type = ref_type::hrel;
            out.offset = off;
            return true;
        }

        static data_ref hrel(int64_t off)
        {
            data_ref out;
            assert(hrel(off, out));
            return out;
        }

        static bool abs(int64_t off, data_ref& out)
        {
            if (off > max_value || off < abs_min_value) return false;
            out.type = ref_type::abs;
            out.offset = off;
            return true;
        }

        static data_ref abs(int64_t off)
        {
            data_ref out;
            assert(abs(off, out));
            return out;
        }

        static bool habs(int64_t off, data_ref& out)
        {
            if (off > max_value || off < abs_min_value) return false;
            out.type = ref_type::habs;
            out.offset = off;
            return true;
        }

        static data_ref habs(int64_t off)
        {
            data_ref out;
            assert(habs(off, out));
            return out;
        }

        static bool parse(int64_t raw, data_ref& out)
        {
            ref_type t = static_cast<ref_type>(raw & 3);
            int64_t off = raw >> 2;
            if ((t == ref_type::rel || t == ref_type::hrel) && (off < rel_min_value || off > max_value))
                return false;
            if ((t == ref_type::abs || t == ref_type::habs) && (off < abs_min_value || off > max_value))
                return false;
            out.type = t;
            out.offset = off;
            return true;
        }

        int64_t encode() const
        {
            return (offset << 2) | static_cast<uint8_t>(type);
        }

        bool operator==(const data_ref& other) const
        {
            return type == other.type && offset == other.offset;
        }

        bool operator!=(const data_ref& other) const
        {
            return !(*this == other);
        }
    };
}
