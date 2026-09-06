#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "../../app_info/assert.hpp"
#include "../../utils/strutils.hpp"
#include "../typed_word.hpp"
#include "../syntax_collection.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            class result
            {
            public:
                uint32_t start{0};
                uint32_t end{0};
                std::string name;
                uint32_t rule_index{0};

                result() = default;

                result(uint32_t s, uint32_t e, std::string n, uint32_t idx)
                    : start(s), end(e), name(std::move(n)), rule_index(idx)
                {
                    assert(end > start);
                    assert(!name.empty());
                }

                bool operator==(const result& other) const
                {
                    return start == other.start &&
                           end == other.end &&
                           name == other.name &&
                           rule_index == other.rule_index;
                }

                bool operator!=(const result& other) const
                {
                    return !(*this == other);
                }

                std::string to_string() const
                {
                    return strcat("[", name, "(", rule_index, "): ", start, "-", end, "]");
                }

                static std::vector<typed_word> typed_words(const std::string& str,
                                                           const std::vector<result>& v,
                                                           const syntax_collection& c)
                {
                    assert(!str.empty());
                    std::vector<typed_word> res;
                    res.reserve(v.size());
                    for (const auto& r : v)
                    {
                        res.emplace_back(str, r.start, r.end, r.rule_index, c.type_name(r.rule_index));
                    }
                    return res;
                }
            };

            class str_result
            {
            public:
                std::string str;
                std::string name;

                str_result() = default;
                str_result(std::string s, std::string n)
                    : str(std::move(s)), name(std::move(n))
                {
                    assert(!str.empty());
                    assert(!name.empty());
                }

                bool operator==(const str_result& other) const
                {
                    return str == other.str && name == other.name;
                }

                bool operator!=(const str_result& other) const
                {
                    return !(*this == other);
                }

                std::string to_string() const
                {
                    return strcat("[", name, ": ", str, "]");
                }

                static std::vector<str_result> of(const std::string& raw, const std::vector<result>& v)
                {
                    assert(!raw.empty());
                    std::vector<str_result> res;
                    res.reserve(v.size());
                    for (const auto& r : v)
                    {
                        assert(r.end <= raw.size());
                        assert(r.end > r.start);
                        res.emplace_back(raw.substr(r.start, r.end - r.start), r.name);
                    }
                    return res;
                }

                static std::vector<str_result> of(const std::vector<std::string>& raw_lines, const std::vector<result>& v)
                {
                    std::string joined;
                    for (size_t i = 0; i < raw_lines.size(); ++i)
                    {
                        joined += raw_lines[i];
                        if (i + 1 < raw_lines.size())
                        {
                            joined += '\n';
                        }
                    }
                    return of(joined, v);
                }
            };
        }
    }
}
