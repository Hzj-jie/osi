#pragma once
#include <string>
#include <cstdio>
#include "characters.hpp"
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            namespace detail
            {
                inline std::string replace_all(std::string str, const std::string& from, const std::string& to)
                {
                    if (from.empty()) return str;
                    size_t start_pos = 0;
                    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
                    {
                        str.replace(start_pos, from.length(), to);
                        start_pos += to.length();
                    }
                    return str;
                }
            }

            inline std::string escape(std::string s)
            {
                if (s.empty()) return s;
                for (char c : characters::all)
                {
                    char hex[3];
                    std::snprintf(hex, sizeof(hex), "%02X", static_cast<unsigned char>(c));
                    std::string from = std::string("\\") + c;
                    std::string to = std::string("\\\\x") + hex;
                    s = detail::replace_all(s, from, to);
                }
                return s;
            }

            inline std::string unescape(std::string s)
            {
                if (s.empty()) return s;
                for (char c : characters::all)
                {
                    char hex_u[3];
                    std::snprintf(hex_u, sizeof(hex_u), "%02X", static_cast<unsigned char>(c));
                    char hex_l[3];
                    std::snprintf(hex_l, sizeof(hex_l), "%02x", static_cast<unsigned char>(c));

                    std::string to(1, c);
                    s = detail::replace_all(s, std::string("\\\\x") + hex_u, to);
                    s = detail::replace_all(s, std::string("\\\\x") + hex_l, to);
                }
                return s;
            }
        }
    }
}
