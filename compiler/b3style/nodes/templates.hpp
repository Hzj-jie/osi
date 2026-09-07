#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "functions.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class template_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "template"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer&) override
                {
                    std::string name;
                    std::shared_ptr<template_template> t;
                    return scope::template_t::of(n, name, t) &&
                           scope::current()->template_table().define(name, t);
                }
            };

            class template_type_name : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "template-type-name"; }

                static std::vector<std::string> extract_param_types(const std::shared_ptr<automata::typed_node>& paramtypelist)
                {
                    std::vector<std::string> types;
                    for (uint32_t i = 0; i < paramtypelist->child_count(); ++i)
                    {
                        auto p = paramtypelist->child(i);
                        if (p->type_name == "type-param-with-comma" ||
                            p->type_name == "paramtype-with-comma" ||
                            p->type_name == "type-name-with-comma")
                        {
                            p = p->child(0);
                        }
                        std::string s;
                        if (code_gen_of(p).dump(s))
                        {
                            while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.pop_back();
                            while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
                            types.push_back(s);
                        }
                        else
                        {
                            types.push_back(p->input_without_ignored());
                        }
                    }
                    return types;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    std::string extended_type;
                    if (!scope::template_t::resolve(n, extended_type))
                    {
                        return false;
                    }
                    return o.append(extended_type);
                }
            };

            inline bool scope::template_t::of(const std::shared_ptr<automata::typed_node>& n,
                                              std::string& name,
                                              std::shared_ptr<template_template>& t)
            {
                assert(n != nullptr && n->child_count() == 2);
                auto head = n->child(0);
                auto body_wrapper = n->child(1);
                assert(body_wrapper->child_count() == 1);
                auto body = body_wrapper->child(0);

                auto type_param_list = head->child(2);
                std::vector<std::string> type_params;
                for (uint32_t i = 0; i < type_param_list->child_count(); ++i)
                {
                    auto p = type_param_list->child(i);
                    if (p->type_name == "type-param-with-comma")
                    {
                        p = p->child(0);
                    }
                    assert(p->type_name == "type-param");
                    type_params.push_back(p->input_without_ignored());
                }

                std::unordered_set<std::string> unique_types(type_params.begin(), type_params.end());
                if (unique_types.size() != type_params.size())
                {
                    raise_error("Duplicate template type parameters in ", head->input());
                    return false;
                }

                std::shared_ptr<automata::typed_node> name_node;
                if (body->type_name == "class")
                {
                    name_node = body->child(1);
                }
                else if (body->type_name == "function")
                {
                    name_node = body->child(1);
                }
                else if (body->type_name == "delegate-with-semi-colon")
                {
                    name_node = body->child(0)->child(2);
                }
                else
                {
                    raise_error("Unsupported template body: ", body->type_name);
                    return false;
                }

                t = std::make_shared<template_template>(body, name_node, type_params);
                name = template_t::name_of(name_node->input_without_ignored(),
                                           static_cast<uint32_t>(type_params.size()));
                return true;
            }

            inline bool scope::template_t::resolve(const std::shared_ptr<automata::typed_node>& n,
                                                   std::string& extended_type_name)
            {
                assert(n != nullptr && n->child_count() == 4);
                std::string raw_name = n->child(0)->input_without_ignored();
                auto types = template_type_name::extract_param_types(n->child(2));
                std::string lookup_name = template_t::name_of(raw_name, static_cast<uint32_t>(types.size()));
                int res = scope::current()->template_table().resolve(lookup_name, types, extended_type_name);
                return res > 0;
            }

            class function_call_with_template : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "function-call-with-template"; }

                static bool split_struct_function(const std::string& name, std::string& obj, std::string& func)
                {
                    size_t dot_pos = name.rfind('.');
                    if (dot_pos == std::string::npos || dot_pos == 0 || dot_pos + 1 >= name.length())
                    {
                        return false;
                    }
                    obj = name.substr(0, dot_pos);
                    func = name.substr(dot_pos + 1);
                    return true;
                }

                static bool build(
                    const std::shared_ptr<automata::typed_node>& n,
                    logic_writer& o,
                    const std::function<bool(const std::string&, const std::shared_ptr<automata::typed_node>&, logic_writer&)>& function_call_build)
                {
                    assert(n != nullptr);
                    assert(function_call_build != nullptr);
                    std::string raw_name = scope::function_name::of(n->child(0)->child(0));
                    std::string obj, func;
                    bool has_dot = split_struct_function(raw_name, obj, func);

                    std::string extended_type;
                    if (!scope::template_t::resolve(n->child(0), extended_type))
                    {
                        return false;
                    }
                    std::string call_target = has_dot ? (obj + "." + extended_type) : extended_type;
                    return function_call_build(has_dot ? call_target : namespace_t::fully_qualified_name(call_target), n, o);
                }

                static bool without_return(const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                {
                    return build(n, o, [](const std::string& fn, const std::shared_ptr<automata::typed_node>& node, logic_writer& oo) {
                        return function_call_node::with_parameters::without_return(fn, node, oo);
                    });
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    return build(n, o, [](const std::string& fn, const std::shared_ptr<automata::typed_node>& node, logic_writer& oo) {
                        return function_call_node::build(fn, node, oo);
                    });
                }

                static bool name_of(const std::shared_ptr<automata::typed_node>& n, std::string& o)
                {
                    assert(n != nullptr && n->child_count() == 4);
                    std::string raw_name = scope::function_name::of(n->child(0));
                    std::string obj, func;
                    std::string function_name = split_struct_function(raw_name, obj, func) ? func : raw_name;
                    o = scope::template_t::name_of(function_name, n->child(2)->child_count());
                    return true;
                }
            };

            class function_name_with_template
            {
            public:
                static const char* node_name() { return "function-name-with-template"; }

                static bool name_of(const std::shared_ptr<automata::typed_node>& n, std::string& o)
                {
                    return function_call_with_template::name_of(n, o);
                }
            };

            class delegate_with_semi_colon_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "delegate-with-semi-colon"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() >= 1);
                    return code_gen_of(n->child(0)).build(o);
                }

                static bool name_node_of(const std::shared_ptr<automata::typed_node>& n, std::shared_ptr<automata::typed_node>& o)
                {
                    assert(n != nullptr);
                    o = n->child(0)->child(2);
                    return true;
                }

                static bool name_of(const std::shared_ptr<automata::typed_node>& n, std::string& o)
                {
                    assert(n != nullptr);
                    std::shared_ptr<automata::typed_node> name_node;
                    name_node_of(n, name_node);
                    o = scope::template_t::name_of(name_node->input_without_ignored(),
                                                   scope::template_t::type_param_count(n));
                    return true;
                }
            };
        }
    }
}
