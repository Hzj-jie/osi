#ifndef DIALECT
#error "DIALECT must be defined before including classes.inc"
#endif
#ifndef WRITER
#error "WRITER must be defined before including classes.inc"
#endif
#ifndef CLASS_VAR_TYPE_OF
#error "CLASS_VAR_TYPE_OF must be defined before including classes.inc"
#endif

namespace osi
{
    namespace compiler
    {
        namespace DIALECT
        {
            class class_node : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "class"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER& o) override
                {
                    assert(n != nullptr && n->child_count() >= 5);

                    std::string class_name = n->child(1)->input_without_ignored();
                    class_def cd(class_name);

                    uint32_t body_start = (n->child(2)->type_name == "class-inheritance") ? 4 : 3;
                    for (uint32_t i = body_start; i + 2 < n->child_count(); ++i)
                    {
                        auto child = n->child(i);
                        if (child->type_name == "struct-body" &&
                            child->child_count() >= 2 &&
                            child->child(0)->type_name == "value-declaration")
                        {
                            auto vd = child->child(0);
                            std::string t = CLASS_VAR_TYPE_OF(vd->child(0));
                            std::string v = scope::variable_name::of(vd->child(1));
                            while (v.rfind(current_namespace_t::namespace_separator, 0) == 0)
                            {
                                v = v.substr(current_namespace_t::namespace_separator.length());
                            }
                            cd.with_var(parameter::non_ref(t, v));
                        }
                    }

                    cd.with_funcs(n);

                    if (n->child(2)->type_name == "class-inheritance")
                    {
                        auto inh_types = n->child(2)->child(1);
                        for (uint32_t i = 0; i < inh_types->child_count(); ++i)
                        {
                            auto tnode = inh_types->child(i);
                            if (tnode->type_name == "type-name-with-comma")
                            {
                                tnode = tnode->child(0);
                            }
                            std::string base_type;
                            if (!code_gen_of(tnode).dump(base_type))
                            {
                                return false;
                            }
                            while (!base_type.empty() && (base_type.back() == ' ' || base_type.back() == '\t')) base_type.pop_back();
                            while (!base_type.empty() && (base_type.front() == ' ' || base_type.front() == '\t')) base_type.erase(base_type.begin());
                            class_def bcd;
                            if (!scope::current()->classes().resolve(base_type, bcd))
                            {
                                raise_error("Base class ", base_type, " not found for class ", class_name);
                                return false;
                            }
                            cd.inherit_from(bcd);
                            cd.with_var(struct_t::create_type_id(base_type));
                        }
                    }

                    if (!cd.check() || !scope::current()->classes().define(class_name, cd))
                    {
                        return false;
                    }

                    std::string code;
                    code += "struct " + class_name + " {";
                    for (const auto& var : cd.vars_)
                    {
                        code += var.non_ref_type() + " " + var.name + ";";
                    }
                    code += "};";

                    for (const auto& f : cd.funcs_)
                    {
                        code += f.content;
                    }
                    for (const auto& t : cd.temps_)
                    {
                        code += t.first + " " + t.second.content;
                    }

                    return build_code(code, o);
                }
            };
        }
    }
}

#undef DIALECT
#undef WRITER
#undef CLASS_VAR_TYPE_OF
