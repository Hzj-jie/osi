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

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            inline std::shared_ptr<automata::nlp> bstyle_nlp()
            {
                static std::shared_ptr<automata::nlp> parser = []() {
                    std::shared_ptr<automata::nlp> p;
                    bool ok = automata::nlp::of(bstyle_lib::nlexer_rule(), bstyle_lib::syntaxer_rule(), p);
                    assert(ok && p != nullptr);
                    return p;
                }();
                return parser;
            }

            inline code_gens<logic_writer>& get_bstyle_code_gens()
            {
                static code_gens<logic_writer> cg = []() {
                    code_gens<logic_writer> c;
                    code_gens_registrar<logic_writer> r;
                    r.with<bool_node>()
                     .with<condition_node>()
                     .with<for_loop_node>()
                     .with<ufloat_node>()
                     .with<function_node>()
                     .with<function_call_node>()
                     .with_delegate("ignore-result-function-call",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return function_call_node::without_return(n->child(0), o);
                         })
                     .with<integer_node>()
                     .with<biguint_node>()
                     .with<logic_node>()
                     .with<multi_sentence_paragraph_node>()
                     .with<param_node>()
                     .with<return_clause_node>()
                     .with<string_node>()
                     .with_delegate("value",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return code_gen_of(n->child(0)).build(o);
                         })
                     .with<value_clause>()
                     .with<value_declaration>()
                     .with<heap_declaration>()
                     .with_delegate("value-definition",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             assert(n->child_count() == 4);
                             return value_declaration::build(n->child(0), n->child(1), o) &&
                                    value_clause::stack_name_build(n->child(1), n->child(3), o);
                         })
                     .with<heap_name>()
                     .with<raw_variable_name>()
                     .with<value_list>()
                     .with<while_node>()
                     .with<include_with_string_node>()
                     .with<include_with_file_node>()
                     .with_named<if_wrapped>("ifdef-wrapped",
                         [](const std::string& s) { return !scope::current()->defines().is_defined(s); })
                     .with_named<if_wrapped>("ifndef-wrapped",
                         [](const std::string& s) { return scope::current()->defines().is_defined(s); })
                     .with<define_node>()
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
                     .with<typedef_type_name_node>()
                     .with<typedef_type_str_node>()
                     .with<struct_node>()
                     .with<reinterpret_cast_node>()
                     .with_delegate("undefine",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr && n->child_count() == 4);
                             std::string name = scope::variable_name::of(n->child(2));
                             return struct_node::undefine(name, o) ||
                                    (builders::of_undefine(name).to(o) &&
                                     scope::current()->variables_undefine(name));
                         })
                     .with_delegate("dealloc",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr && n->child_count() == 4);
                             std::string name = scope::variable_name::of(n->child(2));
                             return struct_node::dealloc_from_heap(name, o) ||
                                    builders::of_dealloc_heap(name).to(o);
                         })
                     .with<static_cast_node>()
                     .with<delegate_node>()
                     .with_delegate("kw-file",
                         [](const std::shared_ptr<automata::typed_node>&, logic_writer& o) {
                             return string_node::build(current_file_guard::get(), o);
                         })
                     .with_delegate("kw-func",
                         [](const std::shared_ptr<automata::typed_node>&, logic_writer& o) {
                             return string_node::build(scope::current()->current_function().signature(), o);
                         })
                     .with_delegate("kw-line",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return integer_node::build(static_cast<int32_t>(n->char_start()), o);
                         })
                     .with_delegate("kw-statement",
                         [](const std::shared_ptr<automata::typed_node>& n, logic_writer& o) {
                             assert(n != nullptr);
                             return string_node::build(n->ancestor_of("sentence")->input(), o);
                         })
                     .with_of_only_childs({
                         "base-root-type",
                         "root-type",
                         "for-increase",
                         "base-for-increase",
                         "paragraph",
                         "sentence",
                         "sentence-with-semi-colon",
                         "variable-name",
                         "value-without-bracket",
                         "base-value-without-bracket",
                         "include",
                         "typedef-type"
                     })
                     .with(code_gen_default::of_ignore_last_child<logic_writer>("root-type-with-semi-colon"))
                     .with(code_gen_default::of_children<logic_writer>("else-condition", {1}))
                     .with(code_gen_default::of_first_child<logic_writer>("param-with-comma"))
                     .with(code_gen_default::of_children<logic_writer>("value-with-bracket", {1}))
                     .with(code_gen_default::of_first_child<logic_writer>("value-with-comma"))
                     .with(code_gen_default::of_all_children<logic_writer>("paramlist"))
                     .with(code_gen_default::of_ignore_last_child<logic_writer>("base-sentence-with-semi-colon"))
                     .with(code_gen_default::of_input_without_ignored<logic_writer>("paramtype"))
                     .with_of_all_childrens({ "paramtypelist" })
                     .with(code_gen_default::of_first_child<logic_writer>("paramtype-with-comma"));

                    r.apply(c);
                    return c;
                }();
                return cg;
            }

            inline bool build_code(const std::string& input, logic_writer& o)
            {
                if (input.empty())
                {
                    return true;
                }
                std::shared_ptr<automata::typed_node> root;
                if (!bstyle_nlp()->parse(input, root))
                {
                    return false;
                }
                assert(root != nullptr);
                assert(root->type == automata::typed_node::root_type);
                assert(root->type_name == automata::typed_node::root_type_name);
                if (root->leaf())
                {
                    return false;
                }
                assert(root->child_count() > 0);
                for (uint32_t i = 0; i < root->child_count(); ++i)
                {
                    if (!code_gen_of(root->child(i)).build(o))
                    {
                        return false;
                    }
                }
                return true;
            }

            inline bool build_full(const std::string& input, logic_writer& o)
            {
                statements<logic_writer> p;
                statements<logic_writer> s;
                code_types::register_statement(p);
                main_caller::register_statement(s);
                scope::call_hierarchy_t::calculator::register_statement(s);

                scope root_scope;
                root_scope.without_end_scope();
                p.export_to(o);
                if (!build_code(input, o))
                {
                    return false;
                }
                s.export_to(o);
                return true;
            }

            class bstyle
            {
            public:
                static const std::string& nlexer_rule()
                {
                    return bstyle_lib::nlexer_rule();
                }

                static const std::string& syntaxer_rule()
                {
                    return bstyle_lib::syntaxer_rule();
                }

                static bool parse(const std::string& input, std::string& o)
                {
                    logic_writer w;
                    if (!build_full(input, w))
                    {
                        return false;
                    }
                    o = w.dump();
                    return true;
                }

                class parse_wrapper
                {
                private:
                    std::shared_ptr<primitive::interrupts> functions_;

                public:
                    explicit parse_wrapper(std::shared_ptr<primitive::interrupts> functions)
                        : functions_(std::move(functions))
                    {
                        assert(functions_ != nullptr);
                    }

                    static current_file_guard with_current_file(const std::string& filename)
                    {
                        return current_file_guard(filename);
                    }

                    bool build(const std::string& input, logic_writer& o)
                    {
                        return build_full(input, o);
                    }

                    bool compile(const std::string& input, primitive::simulator& sim)
                    {
                        if (functions_)
                        {
                            sim.mem().intr() = *functions_;
                        }
                        logic_writer o;
                        if (!build(input, o))
                        {
                            return false;
                        }
                        std::string dumped = o.dump();
                        logic::importer imp(functions_);
                        return imp.import(dumped, sim);
                    }

                    bool compile_file(const std::string& filename, primitive::simulator& sim)
                    {
                        std::ifstream ifs(filename);
                        if (!ifs.is_open())
                        {
                            raise_error("Cannot read content from ", filename);
                            return false;
                        }
                        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                        current_file_guard guard(filename);
                        return compile(content, sim);
                    }
                };

                static parse_wrapper with_functions(std::shared_ptr<primitive::interrupts> functions)
                {
                    return parse_wrapper(std::move(functions));
                }

                static parse_wrapper with_functions(primitive::interrupts functions)
                {
                    return parse_wrapper(std::make_shared<primitive::interrupts>(std::move(functions)));
                }

                static parse_wrapper with_default_functions()
                {
                    return with_functions(std::make_shared<primitive::interrupts>());
                }
            };
        }
    }
}
