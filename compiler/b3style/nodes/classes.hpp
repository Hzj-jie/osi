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
#include "struct_and_values.hpp"
#include "functions.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class class_initializer_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "class-initializer"; }

                static std::string failed_to_build_destructor_message(const std::string& name)
                {
                    assert(!name.empty());
                    return "Failed to build the destructor of the variable " + name +
                           ", this shouldn't happen unless the reinterpret_cast is used which changed the definition of the variable.";
                }

                static std::string failed_to_build_constructor_message(const std::string& class_name,
                                                                      const std::string& var_name)
                {
                    assert(!class_name.empty());
                    assert(!var_name.empty());
                    return class_name + " is not a class or struct, and the variable " +
                           var_name + " cannot be initialized as a class variable.";
                }

                static bool construct(const std::string& name, logic_writer& o)
                {
                    assert(!name.empty());
                    if (!function_call_node::ignore_parameters::without_return(
                            name + "." + scope::class_def::construct, o))
                    {
                        return false;
                    }
                    scope::current()->call_hierarchy().to(
                        namespace_t::fully_qualified_name(scope::class_def::construct));
                    return true;
                }

                static void destruct(const std::string& name, logic_writer& o)
                {
                    scope::current()->when_end_scope([name, &o]() {
                        value_list::with_empty();
                        if (!function_call_node::ignore_parameters::without_return(
                                name + "." + scope::class_def::destruct, o))
                        {
                            raise_error(failed_to_build_destructor_message(name));
                        }
                    });
                    scope::current()->call_hierarchy().to(
                        namespace_t::fully_qualified_name(scope::class_def::destruct));
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && (n->child_count() == 4 || n->child_count() == 5));
                    if (!struct_node::define_in_stack(n->child(0), n->child(1), o))
                    {
                        std::string type = scope::normalized_type::of(n->child(0));
                        std::string name = scope::variable_name::of(n->child(1));
                        if (!value_declaration::declare_primitive_type(type, name, o))
                        {
                            raise_error(failed_to_build_constructor_message(n->child(0)->input(), n->child(1)->input()));
                            return false;
                        }
                    }
                    if (n->child_count() == 4)
                    {
                        value_list::with_empty();
                    }
                    else if (!code_gen_of(n->child(3)).build(o))
                    {
                        return false;
                    }
                    std::string name = scope::fully_qualified_variable_name::of(n->child(1));
                    if (construct(name, o))
                    {
                        destruct(name, o);
                        return true;
                    }
                    return false;
                }
            };

            inline bool value_declaration::build(const std::shared_ptr<automata::typed_node>& n,
                                                 bool class_construct,
                                                 logic_writer& o)
            {
                assert(n != nullptr && n->child_count() >= 2);
                std::string type = scope::normalized_type::of(n->child(0));
                std::string name = scope::fully_qualified_variable_name::of(n->child(1));
                if (!struct_node::define_in_stack(n->child(0), n->child(1), o))
                {
                    return declare_primitive_type(type, name, o);
                }
                if (scope::current()->classes().is_defined(type))
                {
                    if (class_construct)
                    {
                        value_list::with_empty();
                        if (!class_initializer_node::construct(name, o))
                        {
                            return false;
                        }
                    }
                    class_initializer_node::destruct(name, o);
                }
                return true;
            }
        }
    }
}

#define DIALECT b3style_compiler
#define WRITER logic_writer
#define CLASS_VAR_TYPE_OF(n) (scope::normalized_type::of(n))
#include "../../b2style/nodes/classes.inc"
