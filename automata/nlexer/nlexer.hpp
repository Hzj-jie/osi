#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"
#include "../syntax_collection.hpp"
#include "../typed_word.hpp"
#include "characters.hpp"
#include "escape.hpp"
#include "matcher.hpp"
#include "group.hpp"
#include "rule.hpp"
#include "result.hpp"
#include "rule_file.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            class nlexer
            {
            private:
                std::vector<std::pair<std::string, std::shared_ptr<rule>>> rs_;
                syntax_collection sc_;

                static std::string no_match_str(const std::string& i, uint32_t pos)
                {
                    uint32_t half_len = 10;
                    uint32_t start = pos > half_len ? pos - half_len : 0;
                    std::string short_str = i.substr(start, std::min(half_len * 2, static_cast<uint32_t>(i.size() - start)));
                    char ch = pos < i.size() ? i[pos] : '\0';
                    return strcat("[nlexer] No match in ", i, " at pos ", pos, " -> ", short_str, " >> [", ch, "]");
                }

            public:
                explicit nlexer(std::vector<std::pair<std::string, std::shared_ptr<rule>>> rs)
                    : rs_(std::move(rs))
                {
                    assert(!rs_.empty());
                    std::unordered_map<std::string, uint32_t> token_map;
                    for (uint32_t idx = 0; idx < rs_.size(); ++idx)
                    {
                        token_map[rs_[idx].first] = idx;
                    }
                    sc_ = syntax_collection(token_map);
                }

                std::optional<result> match(const std::string& i, uint32_t pos) const
                {
                    std::optional<result> max_res;
                    for (uint32_t j = 0; j < rs_.size(); ++j)
                    {
                        auto r = rs_[j].second->match(i, pos);
                        if (r.has_value() && *r > pos && (!max_res.has_value() || max_res->end < *r))
                        {
                            max_res = result(pos, *r, rs_[j].first, j);
                        }
                    }
                    return max_res;
                }

                bool match(const std::string& i,
                           std::vector<result>& o,
                           const std::vector<std::string>& ignore_types = {}) const
                {
                    o.clear();
                    uint32_t pos = 0;
                    std::unordered_set<std::string> ignores(ignore_types.begin(), ignore_types.end());
                    while (pos < i.size())
                    {
                        auto r = match(i, pos);
                        if (!r.has_value())
                        {
                            raise_error(no_match_str(i, pos));
                            return false;
                        }
                        if (ignores.find(r->name) == ignores.end())
                        {
                            o.push_back(*r);
                        }
                        pos = r->end;
                    }
                    return true;
                }

                bool match(const std::string& i,
                           std::vector<typed_word>& o,
                           const std::vector<std::string>& ignore_types = {}) const
                {
                    std::vector<result> r;
                    if (!match(i, r, ignore_types))
                    {
                        return false;
                    }
                    o = result::typed_words(i, r, sc_);
                    return true;
                }

                result result_of(uint32_t start, uint32_t end, const std::string& name) const
                {
                    for (uint32_t j = 0; j < rs_.size(); ++j)
                    {
                        if (rs_[j].first == name)
                        {
                            return result(start, end, name, j);
                        }
                    }
                    assert(false);
                    return result();
                }

                std::optional<std::vector<result>> match(const std::vector<std::string>& lines,
                                                         const std::vector<std::string>& ignore_types = {}) const
                {
                    std::string joined;
                    for (size_t i = 0; i < lines.size(); ++i)
                    {
                        joined += lines[i];
                        if (i + 1 < lines.size())
                        {
                            joined += '\n';
                        }
                    }
                    std::vector<result> v;
                    if (match(joined, v, ignore_types))
                    {
                        return v;
                    }
                    return std::nullopt;
                }

                static bool of(const std::vector<std::string>& lines, std::shared_ptr<nlexer>& o)
                {
                    rule_file p;
                    if (p.parse(lines))
                    {
                        o = std::make_shared<nlexer>(std::move(p.rules));
                        return true;
                    }
                    return false;
                }

                static bool of(const std::string& content, std::shared_ptr<nlexer>& o)
                {
                    rule_file p;
                    if (p.parse_content(content))
                    {
                        o = std::make_shared<nlexer>(std::move(p.rules));
                        return true;
                    }
                    return false;
                }

                static bool of_file(const std::string& file, std::shared_ptr<nlexer>& o)
                {
                    rule_file p;
                    if (p.parse_file(file))
                    {
                        o = std::make_shared<nlexer>(std::move(p.rules));
                        return true;
                    }
                    return false;
                }

                static std::optional<std::shared_ptr<nlexer>> of(const std::vector<std::string>& lines)
                {
                    std::shared_ptr<nlexer> o;
                    if (of(lines, o))
                    {
                        return o;
                    }
                    return std::nullopt;
                }

                const syntax_collection& str_type_mapping() const
                {
                    return sc_;
                }
            };
        }
    }
}
