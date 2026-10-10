#pragma once
#include <string>
#include <memory>
#include "../../automata/typed_node.hpp"
#include "../logic/builders.hpp"
#include "../logic/scope.hpp"
#include "namespace.hpp"
#include "compiler_scope_interface.hpp"
#include "features.hpp"
#include "accessor.hpp"

namespace osi
{
    namespace compiler
    {
        struct type_name
        {
            static std::string of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return n->input_without_ignored();
            }
        };

        struct variable_name
        {
            static std::string of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                std::string s = n->input_without_ignored();
                size_t pos = 0;
                while ((pos = s.find(".::", pos)) != std::string::npos)
                {
                    s.replace(pos, 3, ".");
                }
                return s;
            }
        };

        struct function_name
        {
            static std::string of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return n->input_without_ignored();
            }
        };

        struct template_name
        {
            static std::string of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return n->input_without_ignored();
            }
        };

        struct normalized_type
        {
            static std::string of(const std::string& type)
            {
                if (type == "type*" ||
                    type == "type_ptr" ||
                    type == "type0" ||
                    type == "::type*" ||
                    type == "::type_ptr" ||
                    type == "::type0")
                {
                    return type;
                }
                auto* cur = compiler_scope_interface::current_or_null();
                if (!cur) return type;
                std::string scoped = current_namespace_t::of(type);
                std::string aliased = cur->type_alias(scoped);
                if (aliased == scoped && type.rfind(current_namespace_t::namespace_separator, 0) != 0)
                {
                    std::string global_scoped = current_namespace_t::namespace_separator + type;
                    std::string global_aliased = cur->type_alias(global_scoped);
                    if (global_aliased != global_scoped)
                    {
                        return global_aliased;
                    }
                }
                return aliased;
            }

            static std::string of(const std::shared_ptr<automata::typed_node>& type)
            {
                assert(type != nullptr);
                return of(type->input_without_ignored());
            }

            static parameter_type parameter_type_of(const std::string& type)
            {
                return parameter_type::of(type).map_type([](const std::string& t) {
                    return of(t);
                });
            }

            static parameter_type parameter_type_of(const std::shared_ptr<automata::typed_node>& type)
            {
                assert(type != nullptr);
                if (type->type_name == "paramtype")
                {
                    bool is_ref = (type->child_count() == 2 && type->child(1)->type_name == "reference");
                    std::string base_t = of(type->child(0));
                    return parameter_type(base_t, is_ref);
                }
                return parameter_type_of(of(type));
            }
        };

        struct fully_qualified_variable_name
        {
            static std::string of(const std::string& name)
            {
                assert(!name.empty());
                std::string s = name;
                size_t pos = 0;
                while ((pos = s.find(".::", pos)) != std::string::npos)
                {
                    s.replace(pos, 3, ".");
                }
                auto* cur = compiler_scope_interface::current_or_null();
                if (cur && cur->is_root())
                {
                    return current_namespace_t::of(s);
                }
                return s;
            }

            static std::string of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return of(variable_name::of(n));
            }
        };

        struct fully_qualified_function_name
        {
            static std::string of(const std::string& name)
            {
                assert(!name.empty());
                if (name == "main")
                {
                    return name;
                }
                return current_namespace_t::of(name);
            }

            static std::string of(const std::shared_ptr<automata::typed_node>& n)
            {
                assert(n != nullptr);
                return of(function_name::of(n));
            }
        };
    }
}
