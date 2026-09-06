#pragma once
#include <string>
#include <string_view>
#include <cctype>

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            namespace characters
            {
                inline constexpr char matching_group_start = '[';
                inline constexpr char matching_group_end = ']';
                inline constexpr char optional_matching = '?';
                inline constexpr char any_matching = '*';
                inline constexpr char multi_matching = '+';
                inline constexpr char matching_group_separator = ',';
                inline constexpr std::string_view matching_separators = " \t\r\n";
                inline constexpr std::string_view reserved_characters = "[]?*+, \t\r\n";

                inline bool valid_type_str(const std::string& s)
                {
                    if (s.empty()) return false;
                    for (char c : s)
                    {
                        if (reserved_characters.find(c) != std::string_view::npos)
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
        }
    }
}
