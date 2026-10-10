#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <cctype>
#include <functional>
#include "../../automata/typed_node.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../logic/logic_writer.hpp"
#include "name_with_namespace.hpp"
#include "namespace.hpp"
#include "root_type_injector.hpp"
#include "accessor.hpp"

namespace osi
{
    namespace compiler
    {
        class compiler_scope_interface;

        class template_template
        {
        public:
            struct extended_type_name_t
            {
                std::string name;
                std::vector<std::string> types;

                explicit extended_type_name_t(std::string n) : name(std::move(n)) {}

                void apply(const std::vector<std::string>& ts)
                {
                    types.clear();
                    for (const auto& t : ts)
                    {
                        std::string cleaned;
                        for (char c : t)
                        {
                            if (std::isalnum(static_cast<unsigned char>(c)) || c == '_')
                            {
                                cleaned += c;
                            }
                            else
                            {
                                cleaned += '_';
                            }
                        }
                        types.push_back(cleaned);
                    }
                }

                std::string str() const
                {
                    std::string res = name;
                    for (const auto& t : types)
                    {
                        res += "__" + t;
                    }
                    return res;
                }
            };

        private:
            std::string template_str_;
            extended_type_name_t ext_name_;
            std::vector<std::string> type_params_;

        public:
            template_template(const std::shared_ptr<automata::typed_node>& body,
                              const std::shared_ptr<automata::typed_node>& name_node,
                              const std::vector<std::string>& types)
                : ext_name_(name_node->input_without_ignored()), type_params_(types)
            {
                assert(body != nullptr);
                assert(name_node != nullptr);
                assert(!types.empty());

                std::stringstream ss;
                body->dfs(
                    [&ss, name_node, &types, this](const std::shared_ptr<automata::typed_node>& node,
                                                  const std::function<void()>& stop_nav) {
                        if (node == name_node)
                        {
                            ss << "@@@NAME@@@ ";
                            stop_nav();
                            return;
                        }
                        if (node->type_name == "raw-type-name")
                        {
                            std::string input_str = node->input_without_ignored();
                            for (size_t i = 0; i < types.size(); ++i)
                            {
                                if (input_str == types[i])
                                {
                                    ss << "@@@PARAM_" << i << "@@@ ";
                                    stop_nav();
                                    return;
                                }
                            }
                        }
                    },
                    [&ss](const std::shared_ptr<automata::typed_node>& leaf) {
                        ss << leaf->word().str() << " ";
                    });
                template_str_ = ss.str();
            }

            std::string extended_type_name(const std::vector<std::string>& types)
            {
                ext_name_.apply(types);
                return ext_name_.str();
            }

            bool apply(const std::vector<std::string>& types, std::string& impl)
            {
                assert(!types.empty());
                assert(types.size() == type_params_.size());
                ext_name_.apply(types);
                std::string res = template_str_;
                std::string name_marker = "@@@NAME@@@";
                size_t pos = 0;
                std::string ext_name_str = ext_name_.str();
                while ((pos = res.find(name_marker, pos)) != std::string::npos)
                {
                    res.replace(pos, name_marker.length(), ext_name_str);
                    pos += ext_name_str.length();
                }
                for (size_t i = 0; i < types.size(); ++i)
                {
                    std::string param_marker = "@@@PARAM_" + std::to_string(i) + "@@@";
                    pos = 0;
                    while ((pos = res.find(param_marker, pos)) != std::string::npos)
                    {
                        res.replace(pos, param_marker.length(), types[i]);
                        pos += types[i].length();
                    }
                }
                while (!res.empty() && res.back() == ' ')
                {
                    res.pop_back();
                }
                impl = res;
                return true;
            }
        };

        class template_t
        {
        public:
            class definition
            {
            public:
                std::shared_ptr<template_template> tmpl;
                std::unordered_set<std::string> injected_types;

                definition() = default;
                explicit definition(std::shared_ptr<template_template> t)
                    : tmpl(std::move(t)) {}

                bool apply(const std::vector<std::string>& types, std::string& o)
                {
                    std::string key;
                    for (const auto& t : types) { key += t + ","; }
                    if (injected_types.insert(key).second)
                    {
                        return tmpl->apply(types, o);
                    }
                    o.clear();
                    return true;
                }

                std::string extended_type_name(const std::vector<std::string>& types)
                {
                    return tmpl->extended_type_name(types);
                }
            };

            using code_builder_t = std::function<bool(const std::string&)>;

        private:
            std::unordered_map<name_with_namespace, definition> m_;
            code_builder_t builder_{nullptr};

        public:
            template_t() = default;
            explicit template_t(code_builder_t b) : builder_(std::move(b)) {}

            void set_code_builder(code_builder_t b) { builder_ = std::move(b); }

            bool define(const std::string& name, std::shared_ptr<template_template> t)
            {
                assert(!name.empty());
                assert(t != nullptr);
                auto nwn = name_with_namespace::of(name);
                if (m_.find(nwn) != m_.end())
                {
                    raise_error("Template [", nwn.fully_qualified_name(), "] has been defined already.");
                    return false;
                }
                m_.emplace(nwn, definition(std::move(t)));
                return true;
            }

            int resolve(const std::string& input_name,
                        const std::vector<std::string>& types,
                        std::string& extended_type_name)
            {
                assert(!types.empty());
                auto nwn = name_with_namespace::of(input_name);
                auto it = m_.find(nwn);
                if (it == m_.end())
                {
                    return 0; // unknown
                }
                std::string s;
                if (!it->second.apply(types, s))
                {
                    return -1; // false
                }
                if (!s.empty())
                {
                    auto* cur = compiler_scope_interface::current_or_null();
                    auto* ns = cur ? cur->myself().current_namespace() : nullptr;
                    auto pop_guard = (nwn.ns.empty() || !ns) ? nullptr : ns->define(nwn.ns);
                    if (builder_ && !builder_(s))
                    {
                        return -1;
                    }
                }
                extended_type_name = namespace_t::fully_qualified_name(nwn.ns, it->second.extended_type_name(types));
                return 1; // true
            }

            static std::string name_of(const std::string& name, uint32_t type_count)
            {
                assert(!name.empty());
                assert(type_count > 0);
                return name + "__" + std::to_string(type_count);
            }

            static uint32_t type_param_count(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr && n->child_count() == 2);
                auto head = n->child(0);
                assert(head != nullptr && head->child_count() == 4);
                assert(!head->child(2)->leaf());
                return head->child(2)->child_count();
            }
        };
    }
}
