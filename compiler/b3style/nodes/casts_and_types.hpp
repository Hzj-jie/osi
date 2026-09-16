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
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "struct_and_values.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class static_cast_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "static-cast"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 6);
                    std::string name = scope::fully_qualified_variable_name::of(n->child(2));
                    std::string type = scope::normalized_type::parameter_type_of(n->child(4)).full_type();
                    if (scope::current()->structs_is_type_defined(type) &&
                        scope::current()->structs_is_variable_defined(name))
                    {
                        raise_error("Cannot static_cast a struct instance of ", name, " to another struct type ", type);
                        return false;
                    }
                    if (scope::current()->structs_is_type_defined(type))
                    {
                        // Convert from type_ptr to struct ptr.
                        struct_def sdef;
                        assert(scope::current()->structs_resolve(type, name, sdef));
                        assert(sdef.primitive_count() > 0);
                        for (const auto& t : sdef.primitives())
                        {
                            if (!value_declaration::declare_primitive_type(t.non_ref_type(), t.name, o)) return false;
                        }
                        if (!builders::start_scope(o).of([&sdef, &name, &o]() -> bool {
                            std::string offset_name = "@offset";
                            if (!value_declaration::declare_primitive_type(logic::scope::type_t::ptr_type, offset_name, o)) return false;
                            for (const auto& t : sdef.primitives())
                            {
                                if (!builders::of_add(t.name, offset_name, name).to(o) ||
                                    !builders::of_add(offset_name, offset_name, "@@prefixes@constants@ptr_offset").to(o))
                                {
                                    return false;
                                }
                            }
                            return true;
                        })) return false;
                        return builders::of_undefine(name).to(o) &&
                               scope::current()->variables_redefine(type, name);
                    }
                    if (scope::current()->structs_is_variable_defined(name))
                    {
                        // Convert from struct ptr to type_ptr.
                        struct_def sdef;
                        assert(scope::current()->structs_resolve_variable(name, sdef));
                        assert(!sdef.primitives().empty());
                        std::string first_prim = sdef.primitives().front().name;
                        return builders::of_define(name, type).to(o) &&
                               builders::of_copy(name, first_prim).to(o) &&
                               sdef.for_each_primitive([&o](const parameter& t) {
                                   return builders::of_undefine(t.name).to(o) &&
                                          scope::current()->variables_undefine(t.name);
                               }) &&
                               scope::current()->variables_redefine(type, name);
                    }
                    raise_error("Unsupported static_cast ", name, " to ", type);
                    return false;
                }
            };

            class reinterpret_cast_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "reinterpret-cast"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 6);
                    std::string name = scope::fully_qualified_variable_name::of(n->child(2));
                    std::string type = scope::normalized_type::parameter_type_of(n->child(4)).full_type();
                    return struct_node::redefine(name, type, o) ||
                           (builders::of_redefine(name, type).to(o) &&
                            scope::current()->variables_redefine(type, name));
                }
            };

            class typedef_type_name_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "typedef-type-name"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    if (n->child_count() == 1)
                    {
                        return code_gen_of(n->child(0)).build(o);
                    }
                    o.append(scope::type_name::of(n));
                    return true;
                }
            };

            class typedef_type_str_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "typedef-type-str"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    std::string s = n->word().str();
                    s = trim(s, "\"");
                    s = c_unescape(s);
                    o.append(s);
                    return true;
                }
            };
        }
    }
}
