#pragma once
#include <cstdint>
#include <string_view>

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            namespace characters
            {
                inline constexpr char matcher_separator = ',';
                inline constexpr char group_separator = '|';
                inline constexpr char group_start = '[';
                inline constexpr char group_end = ']';
                inline constexpr char optional_suffix = '?';
                inline constexpr char _0_or_more_suffix = '*';
                inline constexpr char _1_or_more_suffix = '+';
                inline constexpr char escape_char = '\\';

                inline constexpr char all[] = {
                    matcher_separator,
                    group_separator,
                    group_start,
                    group_end,
                    optional_suffix,
                    _0_or_more_suffix,
                    _1_or_more_suffix
                };
            }
        }
    }
}
