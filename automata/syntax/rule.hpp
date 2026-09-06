#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <cctype>
#include <algorithm>
#include "../../configuration/rule.hpp"
#include "../../app_info/error_handle.hpp"
#include "../syntax_collection.hpp"
#include "characters.hpp"
#include "syntax.hpp"
#include "matching_creator.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class syntaxer;

            class rule : public configuration::rule
            {
            public:
                inline static const std::string command_ignore_types = "IGNORE_TYPES";
                inline static const std::string command_root_types = "ROOT_TYPES";

                std::unordered_set<uint32_t> ignores;
                std::vector<uint32_t> roots;
                syntax_collection collection;

            private:
                std::unordered_map<std::string, std::function<bool(const std::string&)>> cmd_map_;

                static std::vector<std::string> split_types(const std::string& s)
                {
                    std::vector<std::string> v;
                    size_t start = 0;
                    while (true)
                    {
                        size_t pos = s.find(',', start);
                        if (pos == std::string::npos)
                        {
                            v.push_back(s.substr(start));
                            break;
                        }
                        v.push_back(s.substr(start, pos - start));
                        start = pos + 1;
                    }
                    for (auto& str : v)
                    {
                        size_t b = 0;
                        while (b < str.size() && std::isspace(static_cast<unsigned char>(str[b]))) b++;
                        size_t e = str.size();
                        while (e > b && std::isspace(static_cast<unsigned char>(str[e - 1]))) e--;
                        str = str.substr(b, e - b);
                    }
                    v.erase(std::remove_if(v.begin(), v.end(), [](const std::string& item) { return item.empty(); }), v.end());
                    return v;
                }

                bool ignore_types(const std::string& s)
                {
                    ignores.clear();
                    auto v = split_types(s);
                    for (const auto& name : v)
                    {
                        uint32_t j = 0;
                        if (collection.token_type(name, j))
                        {
                            ignores.insert(j);
                        }
                        else
                        {
                            raise_error("cannot find token type ", name, " for ignore types.");
                            return false;
                        }
                    }
                    return true;
                }

                bool root_types(const std::string& s)
                {
                    roots.clear();
                    auto v = split_types(s);
                    for (const auto& name : v)
                    {
                        uint32_t j = 0;
                        if (collection.define(name, j))
                        {
                            roots.push_back(j);
                        }
                        else
                        {
                            raise_error("cannot define syntax type ", name, " for root types.");
                            return false;
                        }
                    }
                    return true;
                }

            public:
                explicit rule(syntax_collection col)
                    : collection(std::move(col))
                {
                    cmd_map_[command_ignore_types] = [this](const std::string& s) { return ignore_types(s); };
                    cmd_map_[command_root_types] = [this](const std::string& s) { return root_types(s); };
                }

                rule() : rule(syntax_collection()) {}

                class exporter;
                exporter export_syntaxer();

            protected:
                const std::unordered_map<std::string, std::function<bool(const std::string&)>>& command_mapping() override
                {
                    return cmd_map_;
                }

                bool default_handler(const std::string& s, const std::string& f) override
                {
                    return syntax::create(f, s, collection);
                }

                bool finish() override
                {
                    return collection.complete();
                }
            };
        }
    }
}
