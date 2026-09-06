#pragma once
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <unordered_map>
#include <functional>
#include "../../configuration/rule.hpp"
#include "../../app_info/error_handle.hpp"
#include "escape.hpp"
#include "rule.hpp"

namespace osi
{
    namespace automata
    {
        namespace nlexer
        {
            class rule_file : public configuration::rule
            {
            public:
                std::vector<std::pair<std::string, std::shared_ptr<osi::automata::nlexer::rule>>> rules;

            protected:
                const std::unordered_map<std::string, std::function<bool(const std::string&)>>& command_mapping() override
                {
                    static const std::unordered_map<std::string, std::function<bool(const std::string&)>> empty_map;
                    return empty_map;
                }

                bool default_handler(const std::string& s, const std::string& f) override
                {
                    if (s.empty() || f.empty())
                    {
                        raise_error("word ", f, " and ", s, " pair has empty definition");
                        return false;
                    }
                    std::shared_ptr<osi::automata::nlexer::rule> r;
                    if (!osi::automata::nlexer::rule::of(escape(s), r))
                    {
                        raise_error("failed to parse rule " + s);
                        return false;
                    }
                    rules.emplace_back(f, std::move(r));
                    return true;
                }
            };
        }
    }
}
