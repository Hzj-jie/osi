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
#define DIALECT bstyle
#include "functions.inc"
#undef DIALECT

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
        }
    }
}
