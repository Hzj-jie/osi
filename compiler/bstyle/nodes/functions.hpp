#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../../function_signature.hpp"
#include "../scope.hpp"
#include "struct_and_values.hpp"
#include "logic_name.hpp"
#include "raw_value.hpp"
#include "../bstyle_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class param_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "param"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer&) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    auto type_node = n->child(0);
                    struct_node::forward_in_stack(type_node->child(0), n->last_child());
                    struct_def params;
                    if (!scope::current()->structs_resolve(scope::type_name::of(type_node->child(0)),
                                                           scope::variable_name::of(n->last_child()),
                                                           params))
                    {
                        params = struct_def::of_primitive(scope::normalized_type::of(type_node->child(0)),
                                                           scope::variable_name::of(n->last_child()));
                    }
                    std::vector<parameter> ps = params.primitives();
                    if (type_node->child_count() == 2)
                    {
                        assert(type_node->child(1)->type_name == "reference");
                        for (auto& p : ps)
                        {
                            p = parameter::to_ref(p);
                        }
                    }
                    scope::current()->params().pack(ps);
                    return true;
                }
            };

            class return_clause_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "return-clause"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 1 || n->child_count() == 2);
                    auto cur_fn = scope::current()->current_function();
                    if (cur_fn.allow_return_value())
                    {
                        if (n->child_count() != 2)
                        {
                            raise_error("Expect return value of ", cur_fn.return_type(), n->child(0)->trace_back_str());
                            return false;
                        }
                    }
                    else
                    {
                        if (n->child_count() != 1)
                        {
                            raise_error("Unexpected return value of ", cur_fn.return_type(), n->child(1)->trace_back_str());
                            return false;
                        }
                    }
                    if (n->child_count() == 1)
                    {
                        return builders::of_return(cur_fn.name()).to(o);
                    }
                    if (!code_gen_of(n->child(1)).build(o))
                    {
                        return false;
                    }
                    auto r = scope::current()->value_target().value();
                    if (cur_fn.return_struct())
                    {
                        if (cur_fn.return_type() != r->type)
                        {
                            raise_error("Return type ", cur_fn.return_type(), " of function ", cur_fn.name(), " does not match ", r->type);
                            return false;
                        }
                        std::string return_value = scope::current()->temp_logic_name().variable() +
                                                   "@" + cur_fn.name() + "@return_value";
                        assert(value_declaration::declare_primitive_type(
                            logic::scope::type_t::variable_type, return_value, o));
                        return struct_node::pack(r->names, return_value, o) &&
                               builders::of_return(cur_fn.name(), return_value).to(o);
                    }
                    if (r->names.size() != 1)
                    {
                        raise_error("Unexpected return value, do not expect a struct to be returned by ", cur_fn.name());
                        return false;
                    }
                    return builders::of_return(cur_fn.name(), r->names[0]).to(o);
                }
            };

            class function_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "function"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    auto new_scope = scope::current()->start_scope();
                    logic_writer fo;
                    bool has_paramlist = (n->child(3)->type_name == "paramlist");
                    if (has_paramlist)
                    {
                        if (!code_gen_of(n->child(3)).build(fo))
                        {
                            return false;
                        }
                    }
                    std::string function_name = scope::function_name::of(n->child(1));
                    std::vector<parameter> params = new_scope->params().unpack();
                    if (!logic_name::of_callee(function_name,
                                               scope::type_name::of(n->child(0)),
                                               params,
                                               [n, &fo, has_paramlist](logic_writer&) -> bool {
                                                   uint32_t gi = has_paramlist ? 5 : 4;
                                                   return code_gen_of(n->child(gi)).build(fo);
                                               },
                                               fo))
                    {
                        return false;
                    }
                    std::string mangled = logic_name::of_function(function_name, params);
                    o.append(new_scope->call_hierarchy().filter(
                        mangled,
                        [fo]() -> std::string {
                            return fo.dump();
                        }));
                    return true;
                }
            };

            class function_call_node : public code_gen<logic_writer>
            {
            private:
                static bool build_impl(
                    const std::shared_ptr<automata::typed_node>& n,
                    logic_writer& o,
                    const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller,
                    const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller_ref)
                {
                    assert(n != nullptr && n->child_count() >= 3);
                    auto bc = [n, build_caller, build_caller_ref](const std::vector<std::string>& parameters) -> bool {
                        std::string raw_name = scope::variable_name::of(n->child(0));
                        if (scope::current()->variables_defined(raw_name))
                        {
                            return build_caller_ref(raw_name, parameters);
                        }
                        std::string name;
                        if (!logic_name::of_function_call(scope::function_name::of(n->child(0)), parameters, name))
                        {
                            return false;
                        }
                        scope::current()->call_hierarchy().to(name);
                        return build_caller(name, parameters);
                    };
                    if (n->child_count() == 3)
                    {
                        return bc({});
                    }
                    if (!code_gen_of(n->child(2)).build(o))
                    {
                        return false;
                    }
                    auto targets = value_list::current_targets();
                    return bc(*targets);
                }

            public:
                static const char* node_name() { return "function-call"; }

                static bool without_return(const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                {
                    return build_impl(n, o,
                        [&o](const std::string& name, const std::vector<std::string>& params) {
                            return builders::of_caller(name, params).to(o);
                        },
                        [&o](const std::string& name, const std::vector<std::string>& params) {
                            return builders::of_caller_ref(name, params).to(o);
                        });
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() >= 3);
                    auto make_caller = [&o](
                        const std::function<bool(const std::string&, const std::string&, const std::vector<std::string>&)>& builder,
                        const std::function<bool(const std::string&, std::string&)>& return_type_of)
                    {
                        return [&o, builder, return_type_of](const std::string& name, const std::vector<std::string>& params) -> bool {
                            std::string return_type;
                            if (!return_type_of(name, return_type)) return false;
                            if (!scope::current()->structs_is_type_defined(return_type))
                            {
                                auto targets = scope::current()->value_target().with_temp_target(return_type, o);
                                assert(!targets.empty());
                                return builder(name, targets.front(), params);
                            }
                            std::string return_value = scope::current()->temp_logic_name().variable() + "@" + name + "@return_value";
                            assert(value_declaration::declare_primitive_type(
                                logic::scope::type_t::variable_type, return_value, o));
                            auto targets = scope::current()->value_target().with_temp_target(return_type, o);
                            return builder(name, return_value, params) &&
                                   struct_node::unpack(return_value, targets, o);
                        };
                    };

                    auto normal_caller = make_caller(
                        [&o](const std::string& name, const std::string& res, const std::vector<std::string>& p) {
                            return builders::of_caller(name, res, p).to(o);
                        },
                        [](const std::string& name, std::string& type) {
                            return scope::current()->functions().return_type_of(name, type);
                        });

                    auto ref_caller = make_caller(
                        [&o](const std::string& name, const std::string& res, const std::vector<std::string>& p) {
                            return builders::of_caller_ref(name, res, p).to(o);
                        },
                        [](const std::string& name, std::string& type) {
                            function_signature<parameter_type> sig;
                            if (!scope::current()->variables_delegate_of(name, sig)) return false;
                            type = sig.return_type;
                            return true;
                        });

                    return build_impl(n, o, normal_caller, ref_caller);
                }
            };

            class delegate_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "delegate"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() >= 5);
                    parameter_type return_type = scope::normalized_type::parameter_type_of(n->child(1));
                    parameter_type name = scope::normalized_type::parameter_type_of(n->child(2));
                    std::vector<parameter_type> ps;
                    if (n->child_count() != 5)
                    {
                        for (const auto& s : get_bstyle_code_gens().of_all_children(n->child(4)).dump())
                        {
                            ps.push_back(scope::normalized_type::parameter_type_of(s));
                        }
                    }
                    std::vector<std::string> param_type_strs;
                    param_type_strs.reserve(ps.size());
                    for (const auto& p : ps)
                    {
                        param_type_strs.push_back(p.full_type());
                    }
                    return scope::current()->delegates().define(return_type.full_type(), name.full_type(), ps) &&
                           builders::of_callee_ref(name.full_type(), return_type.full_type(), param_type_strs).to(o);
                }
            };
        }
    }
}
