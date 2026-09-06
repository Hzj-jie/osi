#pragma once
#include <string>
#include <memory>
#include <optional>
#include <cstdint>
#include "../../app_info/assert.hpp"
#include "characters.hpp"
#include "matcher.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            class group : public matcher
            {
            private:
                std::shared_ptr<matcher> m_;
                std::shared_ptr<matcher> u_;

            public:
                group(std::shared_ptr<matcher> m, std::shared_ptr<matcher> u)
                    : m_(std::move(m)), u_(std::move(u))
                {
                    assert(m_ != nullptr);
                    assert(u_ != nullptr);
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    auto mr = m_->match(i, pos);
                    if (!mr.has_value())
                    {
                        return std::nullopt;
                    }
                    if (u_->match(i, pos).has_value())
                    {
                        return std::nullopt;
                    }
                    return mr;
                }

                static std::shared_ptr<group> of(const std::string& i)
                {
                    size_t sep_pos = i.find(characters::group_separator);
                    if (sep_pos != std::string::npos)
                    {
                        std::string f = i.substr(0, sep_pos);
                        std::string s = i.substr(sep_pos + 1);
                        return std::make_shared<group>(matchers::of(f), matchers::of(s));
                    }
                    return std::make_shared<group>(matchers::of(i), never_matcher::instance());
                }
            };

            class _0_or_more_group : public matcher
            {
            private:
                std::shared_ptr<matcher> g_;

            public:
                explicit _0_or_more_group(std::shared_ptr<matcher> g) : g_(std::move(g))
                {
                    assert(g_ != nullptr);
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    uint32_t r = pos;
                    while (true)
                    {
                        auto n = g_->match(i, r);
                        if (n.has_value() && *n > r)
                        {
                            r = *n;
                        }
                        else
                        {
                            break;
                        }
                    }
                    return r;
                }
            };

            class _1_or_more_group : public matcher
            {
            private:
                std::shared_ptr<matcher> g_;

            public:
                explicit _1_or_more_group(std::shared_ptr<matcher> g) : g_(std::move(g))
                {
                    assert(g_ != nullptr);
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    std::optional<uint32_t> r;
                    while (true)
                    {
                        auto n = g_->match(i, r.has_value() ? *r : pos);
                        if (n.has_value() && (!r.has_value() || *n > *r))
                        {
                            r = n;
                        }
                        else
                        {
                            break;
                        }
                    }
                    return r;
                }
            };

            class optional_group : public matcher
            {
            private:
                std::shared_ptr<matcher> g_;

            public:
                explicit optional_group(std::shared_ptr<matcher> g) : g_(std::move(g))
                {
                    assert(g_ != nullptr);
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    auto r = g_->match(i, pos);
                    if (r.has_value())
                    {
                        return r;
                    }
                    return pos;
                }
            };

            class groups
            {
            public:
                static bool of(const std::string& s, uint32_t& i, std::shared_ptr<matcher>& o)
                {
                    assert(s.size() > i);
                    if (s[i] == characters::group_start)
                    {
                        return of_group(s, i, o);
                    }
                    o = of_raw_string(s, i);
                    return true;
                }

            private:
                static bool of_group(const std::string& s, uint32_t& i, std::shared_ptr<matcher>& o)
                {
                    assert(s[i] == characters::group_start);
                    size_t group_end = s.find(characters::group_end, i);
                    if (group_end == std::string::npos)
                    {
                        return false;
                    }
                    std::string inner = s.substr(i + 1, group_end - i - 1);
                    o = group::of(inner);
                    group_end++;
                    if (group_end < s.size())
                    {
                        char suffix = s[group_end];
                        if (suffix == characters::_0_or_more_suffix)
                        {
                            o = std::make_shared<_0_or_more_group>(o);
                            group_end++;
                        }
                        else if (suffix == characters::_1_or_more_suffix)
                        {
                            o = std::make_shared<_1_or_more_group>(o);
                            group_end++;
                        }
                        else if (suffix == characters::optional_suffix)
                        {
                            o = std::make_shared<optional_group>(o);
                            group_end++;
                        }
                    }
                    assert(o != nullptr);
                    i = static_cast<uint32_t>(group_end);
                    return true;
                }

                static std::shared_ptr<matcher> of_raw_string(const std::string& s, uint32_t& i)
                {
                    assert(s[i] != characters::group_start);
                    uint32_t start = i;
                    size_t next_group_start = s.find(characters::group_start, i);
                    if (next_group_start == std::string::npos)
                    {
                        i = static_cast<uint32_t>(s.size());
                        return group::of(s.substr(start));
                    }
                    i = static_cast<uint32_t>(next_group_start);
                    return group::of(s.substr(start, next_group_start - start));
                }
            };
        }
    }
}
