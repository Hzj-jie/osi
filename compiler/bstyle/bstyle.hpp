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
#include "bstyle_lib.hpp"
#include "bstyle_forward.hpp"
#include "nodes/raw_value.hpp"
#include "nodes/logic_name.hpp"
#include "nodes/struct_and_values.hpp"
#include "nodes/control_flow.hpp"
#include "nodes/functions.hpp"
#include "nodes/casts_and_types.hpp"
#include "nodes/preprocessor.hpp"
#include "nodes/prefixes_suffixes.hpp"

#define DIALECT bstyle_compiler
#include "base_code_gens.inc"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            inline code_gens<logic_writer>& get_bstyle_code_gens()
            {
                static code_gens<logic_writer> cg = []() {
                    code_gens<logic_writer> c;
                    code_gens_registrar<logic_writer> r;
                    register_base_code_gens(r);
                    r.with_delegate("ignore-result-function-call",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return function_call_node::without_return(n->child(0), o);
                         })
                     .with_delegate("value-definition",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr && n->child_count() == 4);
                             return value_declaration::build(n->child(0), n->child(1), o) &&
                                    value_clause::stack_name_build(n->child(1), n->child(3), o);
                         })
                     .with<raw_variable_name>()
                     .with_delegate("typedef",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer&) {
                             assert(n != nullptr && n->child_count() == 3);
                             std::string alias_type, original_type;
                             if (!code_gen_of(n->child(2)).dump(alias_type) ||
                                 !code_gen_of(n->child(1)).dump(original_type))
                             {
                                 return false;
                             }
                             return scope::current()->type_alias().define(alias_type, original_type);
                         })
                     .with_of_only_childs({ "root-type" })
                     .with(code_gen_default::of_input_without_ignored<logic_writer>("paramtype"));

                    r.apply(c);
                    return c;
                }();
                return cg;
            }
        }
    }
}

#define DIALECT bstyle
#define WRITER logic_writer
#include "driver.inc"
