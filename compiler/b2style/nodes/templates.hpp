#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../scope.hpp"
#include "../b2style_forward.hpp"
#include "../../rewriter/typed_node_writer.hpp"
#include "../../code_gen/code_gen.hpp"
#include "functions.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            class template_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "template"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer&) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 2);
                    auto head = n->child(0);
                    auto body_wrapper = n->child(1);
                    assert(body_wrapper->child_count() == 1);
                    auto body = body_wrapper->child(0);

                    // Extract type parameters
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

                    // Check for duplicates
                    std::unordered_set<std::string> unique_types(type_params.begin(), type_params.end());
                    if (unique_types.size() != type_params.size())
                    {
                        raise_error("Duplicate template type parameters in ", head->input());
                        return false;
                    }

                    // Extract name node
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

                    auto tmpl = std::make_shared<template_template>(body, name_node, type_params);
                    std::string tmpl_name = template_t::name_of(name_node->input_without_ignored(),
                                                               static_cast<uint32_t>(type_params.size()));
                    return scope::current()->template_table().define(tmpl_name, tmpl);
                }
            };

            class template_type_name : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "template-type-name"; }

                static std::vector<std::string> extract_param_types(const std::shared_ptr<automata::typed_node>& paramtypelist)
                {
                    std::vector<std::string> types;
                    for (uint32_t i = 0; i < paramtypelist->child_count(); ++i)
                    {
                        auto p = paramtypelist->child(i);
                        if (p->type_name == "paramtype-with-comma")
                        {
                            p = p->child(0);
                        }
                        assert(p->type_name == "paramtype");
                        types.push_back(p->input_without_ignored());
                    }
                    return types;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 4);
                    std::string raw_name = n->child(0)->input_without_ignored();
                    auto types = extract_param_types(n->child(2));

                    std::string lookup_name = template_t::name_of(raw_name, static_cast<uint32_t>(types.size()));
                    std::string extended_type;
                    int res = scope::current()->template_table().resolve(lookup_name, types, extended_type);
                    if (res <= 0)
                    {
                        raise_error("Cannot resolve template ", lookup_name);
                        return false;
                    }
                    o.append(_namespace::bstyle_format::of(extended_type));
                    return true;
                }
            };

            class function_call_with_template : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "function-call-with-template"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 3 || n->child_count() == 4);

                    auto fn_name_with_tmpl = n->child(0);
                    assert(fn_name_with_tmpl->child_count() == 4);
                    std::string full_fn_name = fn_name_with_tmpl->child(0)->input_without_ignored();
                    auto types = template_type_name::extract_param_types(fn_name_with_tmpl->child(2));

                    std::string obj, func;
                    bool has_dot = function_call::split_struct_function(full_fn_name, obj, func);
                    std::string lookup_base = has_dot ? func : full_fn_name;
                    std::string lookup_name = template_t::name_of(lookup_base, static_cast<uint32_t>(types.size()));

                    std::string extended_type;
                    int res = scope::current()->template_table().resolve(lookup_name, types, extended_type);
                    if (res <= 0)
                    {
                        raise_error("Cannot resolve function template ", lookup_name);
                        return false;
                    }

                    std::string call_target;
                    if (has_dot)
                    {
                        call_target = obj + "." + extended_type;
                    }
                    else
                    {
                        call_target = extended_type;
                    }

                    return function_call::build(call_target, n, o);
                }
            };
        }
    }
}
