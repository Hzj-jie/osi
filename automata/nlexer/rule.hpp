#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <cstdint>
#include "../../app_info/assert.hpp"
#include "matcher.hpp"
#include "group.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            class rule : public matcher
            {
            private:
                std::string raw_rule_;
                std::vector<std::shared_ptr<matcher>> ms_;

            public:
                rule(std::string raw_rule, std::vector<std::shared_ptr<matcher>> ms)
                    : raw_rule_(std::move(raw_rule)), ms_(std::move(ms))
                {
                    assert(!ms_.empty());
                }

                explicit rule(std::vector<std::shared_ptr<matcher>> ms)
                    : rule("untracked-raw-rule", std::move(ms))
                {
                }

                std::optional<uint32_t> match(const std::string& i, uint32_t pos) const override
                {
                    for (const auto& m : ms_)
                    {
                        auto r = m->match(i, pos);
                        if (!r.has_value())
                        {
                            return std::nullopt;
                        }
                        pos = *r;
                    }
                    return pos;
                }

                const std::string& raw_rule() const
                {
                    return raw_rule_;
                }

                static bool of(const std::string& s, std::shared_ptr<rule>& o)
                {
                    std::vector<std::shared_ptr<matcher>> ms;
                    uint32_t i = 0;
                    while (i < s.size())
                    {
                        uint32_t j = i;
                        std::shared_ptr<matcher> m;
                        if (!groups::of(s, i, m))
                        {
                            return false;
                        }
                        assert(m != nullptr);
                        assert(i > j);
                        ms.push_back(m);
                    }
                    o = std::make_shared<rule>(s, std::move(ms));
                    return true;
                }
            };
        }
    }
}
