#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "../app_info/assert.hpp"
#include "../utils/strutils.hpp"

namespace osi
{
    namespace automata
    {
        class typed_word
        {
        public:
            constexpr static uint32_t unknown_type = UINT32_MAX;
            inline static const std::string unknown_type_name = "UNKNOWN_TYPE";

            std::string ref;
            uint32_t start{0};
            uint32_t end{0};
            uint32_t len{0};
            uint32_t type{unknown_type};
            std::string type_name{unknown_type_name};

            typed_word() = default;

            typed_word(std::string ref_str,
                       uint32_t start_pos,
                       uint32_t end_pos,
                       uint32_t type_id,
                       std::string type_name_str)
                : ref(std::move(ref_str)),
                  start(start_pos),
                  end(end_pos),
                  len(end_pos >= start_pos ? end_pos - start_pos : 0),
                  type(type_id),
                  type_name(std::move(type_name_str))
            {
                assert(!ref.empty());
                assert(start < end);
                assert(ref.size() >= end);
                assert(!type_name.empty());
            }

            typed_word(std::string ref_str, uint32_t start_pos, uint32_t end_pos, uint32_t type_id)
                : typed_word(std::move(ref_str), start_pos, end_pos, type_id, unknown_type_name)
            {
            }

            typed_word(std::string ref_str, uint32_t start_pos, uint32_t end_pos)
                : typed_word(std::move(ref_str), start_pos, end_pos, unknown_type, unknown_type_name)
            {
            }

            static typed_word fake(uint32_t type)
            {
                return typed_word("fake-typed-word", 0, 1, type, unknown_type_name);
            }

            bool unknown() const
            {
                return type == unknown_type;
            }

            std::string str() const
            {
                return ref.substr(start, len);
            }

            std::string debug_str() const
            {
                return strcat("[", type_name, "]: ", str(), "@", start, "-", end);
            }

            bool operator==(const typed_word& other) const
            {
                return start == other.start &&
                       end == other.end &&
                       type == other.type &&
                       type_name == other.type_name &&
                       str() == other.str();
            }

            bool operator!=(const typed_word& other) const
            {
                return !(*this == other);
            }
        };
    }
}
