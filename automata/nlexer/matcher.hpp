#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <functional>
#include <cctype>
#include "../../app_info/assert.hpp"
#include "characters.hpp"
#include "escape.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            class matcher
            {
            public:
                virtual ~matcher() = default;
                virtual std::optional<uint32_t> match(const std::string& i, uint32_t pos) const = 0;
            };

            class never_matcher : public matcher
            {
            public:
                static const std::shared_ptr<never_matcher>& instance()
                {
                    static const auto inst = std::make_shared<never_matcher>();
                    return inst;
                }

                std::optional<uint32_t> match(const std::string&, uint32_t) const override
                {
                    return std::nullopt;
                }
            };

            class single_char_matcher : public matcher
            {
            public:
                virtual bool check(char c) const { return true; }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    if (pos < i.size() && check(i[pos]))
                    {
                        return pos + 1;
                    }
                    return std::nullopt;
                }
            };

            class string_matcher : public matcher
            {
            private:
                std::string s_;
                uint32_t l_{0};

            public:
                explicit string_matcher(const std::string& s)
                    : s_(unescape(s)), l_(static_cast<uint32_t>(s_.size()))
                {
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    if (i.size() >= pos + l_ && i.compare(pos, l_, s_) == 0)
                    {
                        return pos + l_;
                    }
                    return std::nullopt;
                }

                const std::string& str() const { return s_; }
            };

            class reverse_matcher : public matcher
            {
            private:
                std::shared_ptr<matcher> m_;

            public:
                explicit reverse_matcher(std::shared_ptr<matcher> m) : m_(std::move(m))
                {
                    assert(m_ != nullptr);
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    if (m_->match(i, pos).has_value())
                    {
                        return std::nullopt;
                    }
                    return pos;
                }
            };

            class or_matcher : public matcher
            {
            private:
                std::vector<std::shared_ptr<matcher>> m_;

            public:
                explicit or_matcher(std::vector<std::shared_ptr<matcher>> m) : m_(std::move(m))
                {
                    assert(!m_.empty());
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    std::optional<uint32_t> max_res;
                    for (const auto& matcher_ptr : m_)
                    {
                        auto r = matcher_ptr->match(i, pos);
                        if (r.has_value() && *r > pos && (!max_res.has_value() || *max_res < *r))
                        {
                            max_res = r;
                        }
                    }
                    return max_res;
                }
            };

            class digit_matcher : public single_char_matcher
            {
            public:
                bool check(char c) const override
                {
                    return std::isdigit(static_cast<unsigned char>(c)) != 0;
                }
            };

            class en_char_matcher : public single_char_matcher
            {
            public:
                bool check(char c) const override
                {
                    return std::isalpha(static_cast<unsigned char>(c)) != 0;
                }
            };

            class space_matcher : public single_char_matcher
            {
            public:
                bool check(char c) const override
                {
                    return std::isspace(static_cast<unsigned char>(c)) != 0;
                }
            };

            class newline_matcher : public single_char_matcher
            {
            public:
                bool check(char c) const override
                {
                    return c == '\r' || c == '\n';
                }
            };

            class matchers
            {
            public:
                using creator_t = std::function<std::shared_ptr<matcher>()>;

                static std::unordered_map<std::string, creator_t>& registry()
                {
                    static std::unordered_map<std::string, creator_t> m = []() {
                        std::unordered_map<std::string, creator_t> map;
                        auto any_char = std::make_shared<single_char_matcher>();
                        auto digit = std::make_shared<digit_matcher>();
                        auto not_digit = std::make_shared<reverse_matcher>(digit);
                        auto en_char = std::make_shared<en_char_matcher>();
                        auto not_en_char = std::make_shared<reverse_matcher>(en_char);
                        auto space = std::make_shared<space_matcher>();
                        auto not_space = std::make_shared<reverse_matcher>(space);
                        auto newline = std::make_shared<newline_matcher>();
                        auto not_newline = std::make_shared<reverse_matcher>(newline);

                        map["*"] = [any_char]() { return any_char; };
                        map["\\d"] = [digit]() { return digit; };
                        map["\\D"] = [not_digit]() { return not_digit; };
                        map["\\w"] = [en_char]() { return en_char; };
                        map["\\W"] = [not_en_char]() { return not_en_char; };
                        map["\\b"] = [space]() { return space; };
                        map["\\B"] = [not_space]() { return not_space; };
                        map["\\n"] = [newline]() { return newline; };
                        map["\\N"] = [not_newline]() { return not_newline; };
                        return map;
                    }();
                    return m;
                }

                static void register_matcher(const std::string& s, creator_t f)
                {
                    registry()[s] = std::move(f);
                }

                static std::shared_ptr<matcher> new_matcher(const std::string& i)
                {
                    auto& reg = registry();
                    auto it = reg.find(i);
                    if (it != reg.end())
                    {
                        return it->second();
                    }
                    return std::make_shared<string_matcher>(i);
                }

                static std::shared_ptr<matcher> of(const std::string& i)
                {
                    std::vector<std::string> vs;
                    size_t start = 0;
                    while (true)
                    {
                        size_t pos = i.find(characters::matcher_separator, start);
                        if (pos == std::string::npos)
                        {
                            vs.push_back(i.substr(start));
                            break;
                        }
                        vs.push_back(i.substr(start, pos - start));
                        start = pos + 1;
                    }
                    if (vs.empty())
                    {
                        return never_matcher::instance();
                    }
                    if (vs.size() == 1)
                    {
                        return new_matcher(vs[0]);
                    }
                    std::vector<std::shared_ptr<matcher>> matchers_vec;
                    matchers_vec.reserve(vs.size());
                    for (const auto& s : vs)
                    {
                        matchers_vec.push_back(new_matcher(s));
                    }
                    return std::make_shared<or_matcher>(std::move(matchers_vec));
                }
            };
        }
    }
}
