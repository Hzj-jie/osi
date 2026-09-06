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
#include "../code_gen/includes.hpp"
#include "../rewriter/typed_node_writer.hpp"
#include "../../interpreter/primitive/simulator.hpp"
#include "../../interpreter/primitive/interrupts.hpp"
#include "../bstyle/bstyle.hpp"
#include "b2style_lib.hpp"
#include "b2style_forward.hpp"
#include "scope.hpp"
#include "nodes/multi_sentence_paragraph.hpp"
#include "nodes/expressions.hpp"
#include "nodes/namespaces.hpp"
#include "nodes/classes.hpp"
#include "nodes/functions.hpp"
#include "nodes/templates.hpp"
#include "nodes/preprocessor.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            inline std::shared_ptr<automata::nlp> b2style_nlp()
            {
                static std::shared_ptr<automata::nlp> parser = []() {
                    std::shared_ptr<automata::nlp> p;
                    bool ok = automata::nlp::of(b2style_lib::nlexer_rule(), b2style_lib::syntaxer_rule(), p);
                    assert(ok && p != nullptr);
                    return p;
                }();
                return parser;
            }

            inline code_gens<rewriter::typed_node_writer>& get_b2style_code_gens()
            {
                static code_gens<rewriter::typed_node_writer> cg = []() {
                    code_gens<rewriter::typed_node_writer> c;
                    code_gens_registrar<rewriter::typed_node_writer> r;

                    r.with<multi_sentence_paragraph_node>()
                     .with_delegate("for-loop", [](code_gens<rewriter::typed_node_writer>& this_cg,
                                                   const std::shared_ptr<automata::typed_node>& n,
                                                   rewriter::typed_node_writer& o) {
                         scope child_scope(scope::current());
                         return this_cg.of_all_children(n).build(o);
                     })
                     .with_delegate("struct", [](code_gens<rewriter::typed_node_writer>& this_cg,
                                                 const std::shared_ptr<automata::typed_node>& n,
                                                 rewriter::typed_node_writer& o) {
                         scope child_scope(scope::current());
                         return this_cg.of_all_children(n).build(o);
                     })
                     .with<namespace_node>()
                     .with<heap_struct_name>()
                     .with_named<binary_operation_value>("self-value-clause")
                     .with<binary_operation_value>()
                     .with_named<unary_operation_value>("pre-operation-value", 0, "_pre")
                     .with_named<unary_operation_value>("post-operation-value", 1, "_post")
                     .with<function_call>()
                     .with_named<function_call>("heap-struct-function-call")
                     .with<include_with_string_node>()
                     .with<include_with_file_node>()
                     .with<class_node>()
                     .with<template_node>()
                     .with<template_type_name>()
                     .with<function_call_with_template>()
                     .with<name_node>()
                     .with_named<name_node>("raw-type-name")
                     .with<function_node>()
                     .with(code_gen_default::of_first_child<rewriter::typed_node_writer>("type-param-with-comma"))
                     .with(code_gen_default::of_first_child<rewriter::typed_node_writer>("type-name-with-comma"))
                     .with(code_gen_default::of_only_descendant_str<rewriter::typed_node_writer>("type-param"))
                     .with(code_gen_default::of_only_descendant_str<rewriter::typed_node_writer>("reference"))
                     .with_delegate("value-definition", [](code_gens<rewriter::typed_node_writer>& this_cg,
                                                           const std::shared_ptr<automata::typed_node>& n,
                                                           rewriter::typed_node_writer& o) {
                         assert(n != nullptr);
                         if (!variable_proxy::define()(n)) return false;
                         if (!call_hierarchy_t::from_value_clause()(n)) return false;
                         return this_cg.of_all_children(n).build(o);
                     })
                     .with_delegate("value-declaration", [](code_gens<rewriter::typed_node_writer>& this_cg,
                                                            const std::shared_ptr<automata::typed_node>& n,
                                                            rewriter::typed_node_writer& o) {
                         assert(n != nullptr);
                         if (!variable_proxy::define()(n)) return false;
                         return this_cg.of_all_children(n).build(o);
                     })
                     .with_delegate("value-clause", [](code_gens<rewriter::typed_node_writer>& this_cg,
                                                       const std::shared_ptr<automata::typed_node>& n,
                                                       rewriter::typed_node_writer& o) {
                         assert(n != nullptr);
                         if (!call_hierarchy_t::from_value_clause()(n)) return false;
                         return this_cg.of_all_children(n).build(o);
                     })
                     .with_named<if_wrapped>("ifdef-wrapped",
                         [](const std::string& s) { return !scope::current()->defines().is_defined(s); })
                     .with_named<if_wrapped>("ifndef-wrapped",
                         [](const std::string& s) { return scope::current()->defines().is_defined(s); })
                     .with<define_node>()
                     .with<paramtype_with_comma>()
                     .with<class_initializer>()
                     .with(code_gen_default::of_ignore<rewriter::typed_node_writer>("colon"))
                     .with_delegate("root-type", [](code_gens<rewriter::typed_node_writer>& this_cg,
                                                    const std::shared_ptr<automata::typed_node>& n,
                                                    rewriter::typed_node_writer& o) {
                         assert(n != nullptr && n->child_count() == 1);
                         scope::current()->root_type_injector()._new(o);
                         return this_cg.of(n->child(0)).build(o);
                     })
                     .with_delegate("kw-statement", [](const std::shared_ptr<automata::typed_node>& n,
                                                       rewriter::typed_node_writer& o) {
                         assert(n != nullptr);
                         const auto* sentence = n->ancestor_of("sentence");
                         assert(sentence != nullptr);
                         return o.append("\"" + c_escape(sentence->input()) + "\"");
                     })
                     .with_delegate("kw-file", [](const std::shared_ptr<automata::typed_node>&,
                                                  rewriter::typed_node_writer& o) {
                         return o.append("\"" + c_escape(bstyle_compiler::current_file_guard::get()) + "\"");
                     })
                     .with(code_gen_default::of_input<rewriter::typed_node_writer>("kw-func"))
                     .with(code_gen_default::of_input<rewriter::typed_node_writer>("kw-line"))
                     .with_of_only_childs({
                         "base-root-type",
                         "paragraph",
                         "sentence-with-semi-colon",
                         "ignore-result-function-call",
                         "ignore-result-heap-struct-function-call",
                         "ignore-result-function-call-with-template",
                         "for-increase",
                         "base-for-increase",
                         "base-value-without-bracket",
                         "typedef-type",
                         "type-name",
                         "unary-operation-value"
                     })
                     .with_of_all_childrens({
                         "root-type-with-semi-colon",
                         "paramlist",
                         "param-with-comma",
                         "param",
                         "sentence",
                         "base-sentence-with-semi-colon",
                         "b2style-sentence-with-semi-colon",
                         "heap-declaration",
                         "heap-name",
                         "return-clause",
                         "logic",
                         "condition",
                         "while",
                         "value",
                         "else-condition",
                         "value-with-bracket",
                         "raw-value",
                         "value-without-bracket",
                         "value-with-operation",
                         "variable-name",
                         "value-list",
                         "value-with-comma",
                         "include",
                         "typedef-type-name",
                         "typedef-type-str",
                         "typedef",
                         "type-param-list",
                         "reinterpret-cast",
                         "delegate",
                         "paramtypelist",
                         "paramtype",
                         "struct-body",
                         "undefine",
                         "dealloc",
                         "static-cast"
                     });

                    std::vector<std::string> leaf_names = {
                        "kw-if",
                        "kw-else",
                        "kw-for",
                        "kw-while",
                        "kw-do",
                        "kw-loop",
                        "kw-return",
                        "kw-break",
                        "kw-logic",
                        "kw-reinterpret-cast",
                        "kw-undefine",
                        "kw-dealloc",
                        "kw-static-cast",
                        "start-square-bracket",
                        "end-square-bracket",
                        "bool",
                        "integer",
                        "biguint",
                        "ufloat",
                        "string",
                        "semi-colon",
                        "comma",
                        "start-paragraph",
                        "end-paragraph",
                        "start-bracket",
                        "end-bracket",
                        "assignment",
                        "kw-typedef",
                        "kw-struct",
                        "kw-delegate"
                    };
                    for (const auto& name : leaf_names)
                    {
                        r.with_delegate(name, [](const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) {
                            assert(n != nullptr && n->leaf());
                            return o.append(n);
                        });
                    }

                    r.apply(c);
                    return c;
                }();
                return cg;
            }

            inline bool build_code(const std::string& input, rewriter::typed_node_writer& o)
            {
                if (input.empty())
                {
                    return true;
                }
                const std::string* pinput = &input;
                std::string stripped;
                if (input.rfind("\xef\xbb\xbf", 0) == 0)
                {
                    stripped = input.substr(3);
                    pinput = &stripped;
                }
                if (pinput->empty())
                {
                    return true;
                }
                std::shared_ptr<automata::typed_node> root;
                if (!b2style_nlp()->parse(*pinput, root))
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

            inline bool build_full(const std::string& input, rewriter::typed_node_writer& o)
            {
                statements<rewriter::typed_node_writer> s;
                call_hierarchy_t::calculator::register_statement(s);

                scope root_scope;
                root_scope.disable_end_scope();
                if (!build_code(input, o))
                {
                    return false;
                }
                s.export_to(o);
                return true;
            }

            class b2style
            {
            public:
                static const std::string& nlexer_rule()
                {
                    return b2style_lib::nlexer_rule();
                }

                static const std::string& syntaxer_rule()
                {
                    return b2style_lib::syntaxer_rule();
                }

                static bool parse(const std::string& input, std::string& o)
                {
                    rewriter::typed_node_writer w;
                    if (!build_full(input, w))
                    {
                        return false;
                    }
                    o = w.dump();
                    return true;
                }

                class compile_wrapper
                {
                private:
                    std::shared_ptr<primitive::interrupts> functions_;

                public:
                    explicit compile_wrapper(std::shared_ptr<primitive::interrupts> functions)
                        : functions_(std::move(functions))
                    {
                        assert(functions_ != nullptr);
                    }

                    static bstyle_compiler::current_file_guard with_current_file(const std::string& filename)
                    {
                        return bstyle_compiler::current_file_guard(filename);
                    }

                    bool build(const std::string& input, rewriter::typed_node_writer& o)
                    {
                        return build_full(input, o);
                    }

                    bool compile(const std::string& input, primitive::simulator& sim)
                    {
                        rewriter::typed_node_writer o;
                        if (!build(input, o))
                        {
                            return false;
                        }
                        std::string bstyle_code = o.dump();
                        return bstyle_compiler::bstyle::with_functions(functions_).compile(bstyle_code, sim);
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
                        bstyle_compiler::current_file_guard guard(filename);
                        return compile(content, sim);
                    }
                };

                static compile_wrapper with_functions(std::shared_ptr<primitive::interrupts> functions)
                {
                    return compile_wrapper(std::move(functions));
                }

                static compile_wrapper with_functions(primitive::interrupts functions)
                {
                    return compile_wrapper(std::make_shared<primitive::interrupts>(std::move(functions)));
                }

                static compile_wrapper with_default_functions()
                {
                    return with_functions(std::make_shared<primitive::interrupts>());
                }
            };
        }
    }
}
