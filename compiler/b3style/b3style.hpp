#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../automata/nlp.hpp"
#include "../../automata/typed_node.hpp"
#include "../code_gen/code_gen.hpp"
#include "../code_gen/code_gen.default.hpp"
#include "../code_gen/code_gens_registrar.hpp"
#include "../code_gen/statements.hpp"
#include "../logic/logic_writer.hpp"
#include "../logic/builders.hpp"
#include "../logic/importer.hpp"
#include "../../interpreter/primitive/simulator.hpp"
#include "../../interpreter/primitive/interrupts.hpp"
#include "../../interpreter/primitive/console_io.hpp"
#include "scope.hpp"
#include "b3style_lib.hpp"
#include "b3style_forward.hpp"
#include "nodes/prefixes_suffixes.hpp"
#include "nodes/logic_name.hpp"
#include "nodes/raw_value.hpp"
#include "nodes/struct_and_values.hpp"
#include "nodes/functions.hpp"
#include "nodes/classes.hpp"
#include "nodes/control_flow.hpp"
#include "nodes/casts_and_types.hpp"
#include "nodes/operations.hpp"
#include "nodes/namespaces.hpp"
#include "nodes/templates.hpp"
#include "nodes/preprocessor.hpp"

#define DIALECT b3style_compiler
#include "../bstyle/base_code_gens.inc"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            inline code_gens<logic_writer>& get_b3style_code_gens()
            {
                static code_gens<logic_writer> cg = []() {
                    code_gens<logic_writer> c;
                    code_gens_registrar<logic_writer> r;
                    register_base_code_gens(r);

                    r.with_of_only_childs({
                         "unary-operation-value",
                         "type-name"
                     })
                     .with_of_all_childrens({
                         "raw-value",
                         "paramtype",
                         "value-with-operation"
                     })
                     .with(code_gen_default::of_ignore_last_child<logic_writer>("b2style-sentence-with-semi-colon"))
                     .with(code_gen_default::of_only_descendant_str<logic_writer>("reference"))
                     .with<heap_struct_function_call_node>()
                     .with_delegate("ignore-result-heap-struct-function-call",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return heap_struct_function_call_node::with_parameters::without_return(n->child(0), o);
                         })
                     .with_delegate("value-definition",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr && n->child_count() == 4);
                             return value_declaration::without_class_construct(n, o) &&
                                    value_clause::stack_name_build(n->child(1), n->child(3), o);
                         })
                     .with_delegate("typedef",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer&) {
                             assert(n != nullptr && n->child_count() == 3);
                             std::string alias_type = scope::type_name::of(n->child(2));
                             std::string original_type;
                             if (!code_gen_of(n->child(1)).dump(original_type))
                             {
                                 return false;
                             }
                             return scope::current()->type_alias().define(alias_type, original_type);
                         })
                     .with<name_node>()
                     .with<namespace_node>()
                     .with<binary_operation_value_node>()
                     .with_named<unary_operation_value_node>("pre-operation-value", 0, "_pre")
                     .with_named<unary_operation_value_node>("post-operation-value", 1, "_post")
                     .with_delegate("self-value-clause", &binary_operation_value_node::without_return)
                     .with<class_node>()
                     .with<class_initializer_node>()
                     .with<template_node>()
                     .with(code_gen_default::of_first_child<logic_writer>("type-param-with-comma"))
                     .with(code_gen_default::of_only_descendant_str<logic_writer>("type-param"))
                     .with<function_call_with_template>()
                     .with_delegate("ignore-result-function-call-with-template",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return function_call_with_template::without_return(n->child(0), o);
                         })
                     .with_delegate("root-type",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr && n->child_count() == 1);
                             auto g = scope::current()->root_type_injector()._new(o);
                             return code_gen_of(n->child(0)).build(o);
                         })
                     .with<delegate_with_semi_colon_node>()
                     .with_delegate("raw-type-name",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return o.append(scope::normalized_type::of(n));
                         })
                     .with(code_gen_default::of_first_child<logic_writer>("type-name-with-comma"))
                     .with<template_type_name>()
                     .with<heap_struct_name_node>();

                    r.apply(c);
                    return c;
                }();
                return cg;
            }
        }
    }
}

#define DIALECT b3style
#define WRITER logic_writer
#define DRIVER_EXTRA_MEMBERS \
    using class_initializer = class_initializer_node; \
    static auto disable_namespace() { return b3style_compiler::disable_namespace(); }
#include "../bstyle/driver.inc"
