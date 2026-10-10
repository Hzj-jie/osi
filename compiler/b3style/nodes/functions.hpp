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
#include "../b3style_forward.hpp"
#include "struct_and_values.hpp"
#include "logic_name.hpp"
#include "raw_value.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
#define DIALECT b3style
#include "../../bstyle/nodes/functions.inc"
#undef DIALECT

            template <typename NameBuilder>
            class function_call_impl : public code_gen<logic_writer>
            {
            private:
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

                static bool build_function_caller(
                    const std::string& function_name,
                    const std::vector<std::string>& parameters,
                    const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller)
                {
                    assert(!function_name.empty());
                    assert(build_caller != nullptr);
                    std::string name;
                    if (!logic_name::of_function_call(function_name, parameters, name))
                    {
                        return false;
                    }
                    if (!scope::current()->functions().is_defined(name))
                    {
                        return false;
                    }
                    scope::current()->call_hierarchy().to(name);
                    return build_caller(name, parameters);
                }

            public:
                static bool build(
                    std::string raw_function_name,
                    const std::shared_ptr<automata::typed_node>& name_node,
                    const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller,
                    const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller_ref,
                    logic_writer& o)
                {
                    assert(build_caller != nullptr && build_caller_ref != nullptr);
                    assert(!raw_function_name.empty() || name_node != nullptr);
                    if (raw_function_name.empty())
                    {
                        raw_function_name = scope::function_name::of(name_node);
                    }
                    assert(!raw_function_name.empty());
                    auto targets = value_list::current_targets();
                    std::vector<std::string> parameters = *targets;
                    std::string obj, func;
                    if (split_struct_function(raw_function_name, obj, func))
                    {
                        if (!NameBuilder::run(obj, name_node, o))
                        {
                            raise_error("Cannot find class instance ", obj);
                            return false;
                        }
                        std::vector<std::string> all_params;
                        {
                            auto val = scope::current()->value_target().value();
                            all_params = val->names;
                        }
                        all_params.insert(all_params.end(), parameters.begin(), parameters.end());
                        return build_function_caller(namespace_t::fully_qualified_name(func), all_params, build_caller);
                    }
                    if (scope::current()->variables_defined(raw_function_name))
                    {
                        return build_caller_ref(raw_function_name, parameters);
                    }
                    return build_function_caller(scope::fully_qualified_function_name::of(raw_function_name),
                                                 parameters,
                                                 build_caller);
                }

                static std::function<bool(const std::string&, const std::vector<std::string>&)>
                without_return_caller_builder(logic_writer& o)
                {
                    return [&o](const std::string& name, const std::vector<std::string>& parameters) {
                        return builders::of_caller(name, parameters).to(o);
                    };
                }

                static std::function<bool(const std::string&, const std::vector<std::string>&)>
                without_return_caller_ref_builder(logic_writer& o)
                {
                    return [&o](const std::string& name, const std::vector<std::string>& parameters) {
                        return builders::of_caller_ref(name, parameters).to(o);
                    };
                }

                static std::function<bool(const std::string&, const std::vector<std::string>&)>
                builder(const std::function<bool(const std::string&, const std::string&, const std::vector<std::string>&)>& logic_builder,
                        const std::function<bool(const std::string&, std::string&)>& return_type_of,
                        logic_writer& o)
                {
                    assert(logic_builder != nullptr && return_type_of != nullptr);
                    return [&o, logic_builder, return_type_of](const std::string& name, const std::vector<std::string>& parameters) -> bool {
                        std::string return_type;
                        if (!return_type_of(name, return_type))
                        {
                            return false;
                        }
                        if (!scope::current()->structs_is_type_defined(return_type))
                        {
                            auto targets = scope::current()->value_target().with_temp_target(return_type, o);
                            assert(!targets.empty());
                            return logic_builder(name, targets.front(), parameters);
                        }
                        std::string return_value = scope::current()->temp_logic_name().variable() + "@" + name + "@return_value";
                        assert(value_declaration::declare_primitive_type(
                            logic::scope::type_t::variable_type, return_value, o));
                        auto targets = scope::current()->value_target().with_temp_target(return_type, o);
                        return logic_builder(name, return_value, parameters) &&
                               struct_node::unpack(return_value, targets, o);
                    };
                }

                static std::function<bool(const std::string&, const std::vector<std::string>&)>
                caller_builder(logic_writer& o)
                {
                    return builder(
                        [&o](const std::string& name, const std::string& result, const std::vector<std::string>& params) {
                            return builders::of_caller(name, result, params).to(o);
                        },
                        [](const std::string& name, std::string& type) {
                            return scope::current()->functions().return_type_of(name, type);
                        },
                        o);
                }

                static std::function<bool(const std::string&, const std::vector<std::string>&)>
                caller_ref_builder(logic_writer& o)
                {
                    return builder(
                        [&o](const std::string& name, const std::string& result, const std::vector<std::string>& params) {
                            return builders::of_caller_ref(name, result, params).to(o);
                        },
                        [](const std::string& name, std::string& type) {
                            function_signature<parameter_type> signature;
                            if (!scope::current()->variables_delegate_of(name, signature))
                            {
                                return false;
                            }
                            type = signature.return_type;
                            return true;
                        },
                        o);
                }

                struct ignore_parameters
                {
                    static bool without_return(const std::string& function_name, logic_writer& o)
                    {
                        return function_call_impl::build(function_name,
                                                         nullptr,
                                                         without_return_caller_builder(o),
                                                         without_return_caller_ref_builder(o),
                                                         o);
                    }

                    static bool build(const std::string& function_name, logic_writer& o)
                    {
                        return function_call_impl::build(function_name,
                                                         nullptr,
                                                         caller_builder(o),
                                                         caller_ref_builder(o),
                                                         o);
                    }
                };

                struct with_parameters
                {
                    static bool build(const std::string& name,
                                     const std::shared_ptr<automata::typed_node>& n,
                                     const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller,
                                     const std::function<bool(const std::string&, const std::vector<std::string>&)>& build_caller_ref,
                                     logic_writer& o)
                    {
                        assert(n != nullptr && n->child_count() >= 3);
                        if (n->child_count() == 3)
                        {
                            value_list::with_empty();
                        }
                        else if (!code_gen_of(n->child(2)).build(o))
                        {
                            return false;
                        }
                        return function_call_impl::build(name, n->child(0), build_caller, build_caller_ref, o);
                    }

                    static bool without_return(const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                    {
                        return build("", n, without_return_caller_builder(o), without_return_caller_ref_builder(o), o);
                    }

                    static bool without_return(const std::string& function_name,
                                              const std::shared_ptr<automata::typed_node>& n,
                                              logic_writer& o)
                    {
                        return build(function_name, n, without_return_caller_builder(o), without_return_caller_ref_builder(o), o);
                    }

                    static bool build(const std::string& function_name,
                                     const std::shared_ptr<automata::typed_node>& n,
                                     logic_writer& o)
                    {
                        return build(function_name, n, caller_builder(o), caller_ref_builder(o), o);
                    }

                    static bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                    {
                        return build("", n, o);
                    }
                };

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    return with_parameters::build(n, o);
                }

                static bool build(const std::string& function_name,
                                  const std::shared_ptr<automata::typed_node>& n,
                                  logic_writer& o)
                {
                    return with_parameters::build(function_name, n, o);
                }
            };

            struct raw_variable_name_of
            {
                static bool run(const std::string& name, const std::shared_ptr<automata::typed_node>&, logic_writer&)
                {
                    return raw_variable_name::build(name);
                }
            };

            struct heap_struct_name_of
            {
                static bool run(const std::string&, const std::shared_ptr<automata::typed_node>& n, logic_writer& o)
                {
                    assert(n != nullptr && n->child_count() > 0);
                    return code_gen_of(n->child(0)).build(o);
                }
            };

            class function_call_node : public function_call_impl<raw_variable_name_of>
            {
            public:
                static const char* node_name() { return "function-call"; }
            };

            class heap_struct_function_call_node : public function_call_impl<heap_struct_name_of>
            {
            public:
                static const char* node_name() { return "heap-struct-function-call"; }
            };
        }
    }
}
