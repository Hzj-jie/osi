#ifndef DIALECT
#error "DIALECT must be defined before including base_code_gens.inc"
#endif

namespace osi
{
    namespace compiler
    {
        namespace DIALECT
        {
            inline void register_base_code_gens(code_gens_registrar<logic_writer>& r)
            {
                r.with<bool_node>()
                 .with<condition_node>()
                 .with<for_loop_node>()
                 .with<ufloat_node>()
                 .with<function_node>()
                 .with<function_call_node>()
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
                 .with<heap_name>()
                 .with<struct_node>()
                 .with<value_list>()
                 .with<while_node>()
                 .with<include_with_string_node>()
                 .with<include_with_file_node>()
                 .with_named<if_wrapped>("ifdef-wrapped",
                     [](const std::string& s) { return !scope::current()->defines().is_defined(s); })
                 .with_named<if_wrapped>("ifndef-wrapped",
                     [](const std::string& s) { return scope::current()->defines().is_defined(s); })
                 .with<define_node>()
                 .with<typedef_type_name_node>()
                 .with<typedef_type_str_node>()
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
                         const auto* sentence = n->ancestor_of("sentence");
                         assert(sentence != nullptr);
                         return string_node::build(sentence->input(), o);
                     })
                 .with_of_only_childs({
                     "base-root-type",
                     "include",
                     "typedef-type",
                     "paragraph",
                     "sentence",
                     "sentence-with-semi-colon",
                     "value-without-bracket",
                     "base-value-without-bracket",
                     "variable-name",
                     "for-increase",
                     "base-for-increase"
                 })
                 .with_of_all_childrens({
                     "paramtypelist",
                     "paramlist"
                 })
                 .with(code_gen_default::of_ignore_last_child<logic_writer>("root-type-with-semi-colon"))
                 .with(code_gen_default::of_ignore_last_child<logic_writer>("base-sentence-with-semi-colon"))
                 .with(code_gen_default::of_first_child<logic_writer>("param-with-comma"))
                 .with(code_gen_default::of_first_child<logic_writer>("value-with-comma"))
                 .with(code_gen_default::of_first_child<logic_writer>("paramtype-with-comma"))
                 .with(code_gen_default::of_children<logic_writer>("else-condition", {1}))
                 .with(code_gen_default::of_children<logic_writer>("value-with-bracket", {1}));
            }
        }
    }
}

#undef DIALECT
