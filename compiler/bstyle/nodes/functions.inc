#ifndef DIALECT
#error "DIALECT must be defined before including functions.inc"
#endif

#define GLUE_HELPER(x, y) x##y
#define GLUE(x, y) GLUE_HELPER(x, y)
#define GET_CODE_GENS GLUE(get_, GLUE(DIALECT, _code_gens))

            class param_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "param"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer&) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    auto type_node = n->child(0);
                    struct_node::forward_in_stack(type_node->child(0), n->last_child());
                    struct_def params;
                    if (!scope::current()->structs_resolve(scope::normalized_type::of(type_node->child(0)),
                                                           scope::variable_name::of(n->last_child()),
                                                           params))
                    {
                        params = struct_def::of_primitive(scope::normalized_type::of(type_node->child(0)),
                                                           scope::variable_name::of(n->last_child()));
                    }
                    std::vector<parameter> ps = params.primitives();
                    if (type_node->child_count() == 2)
                    {
                        assert(type_node->child(1)->type_name == "reference");
                        for (auto& p : ps)
                        {
                            p = parameter::to_ref(p);
                        }
                    }
                    scope::current()->params().pack(ps);
                    return true;
                }
            };

            class return_clause_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "return-clause"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 1 || n->child_count() == 2);
                    auto cur_fn = scope::current()->current_function();
                    if (cur_fn.allow_return_value())
                    {
                        if (n->child_count() != 2)
                        {
                            raise_error("Expect return value of ", cur_fn.return_type(), n->child(0)->trace_back_str());
                            return false;
                        }
                    }
                    else
                    {
                        if (n->child_count() != 1)
                        {
                            raise_error("Unexpected return value of ", cur_fn.return_type(), n->child(1)->trace_back_str());
                            return false;
                        }
                    }
                    if (n->child_count() == 1)
                    {
                        return builders::of_return(cur_fn.name()).to(o);
                    }
                    if (!code_gen_of(n->child(1)).build(o))
                    {
                        return false;
                    }
                    auto r = scope::current()->value_target().value();
                    if (cur_fn.return_struct())
                    {
                        if (cur_fn.return_type() != r->type)
                        {
                            raise_error("Return type ", cur_fn.return_type(), " of function ", cur_fn.name(), " does not match ", r->type);
                            return false;
                        }
                        std::string return_value = scope::current()->temp_logic_name().variable() +
                                                   "@" + cur_fn.name() + "@return_value";
                        assert(value_declaration::declare_primitive_type(
                            logic::scope::type_t::variable_type, return_value, o));
                        return struct_node::pack(r->names, return_value, o) &&
                               builders::of_return(cur_fn.name(), return_value).to(o);
                    }
                    if (r->names.size() != 1)
                    {
                        raise_error("Unexpected return value, do not expect a struct to be returned by ", cur_fn.name());
                        return false;
                    }
                    return builders::of_return(cur_fn.name(), r->names[0]).to(o);
                }
            };

            class function_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "function"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    auto new_scope = scope::current()->start_scope();
                    logic_writer fo;
                    bool has_paramlist = (n->child(3)->type_name == "paramlist");
                    if (has_paramlist)
                    {
                        if (!code_gen_of(n->child(3)).build(fo))
                        {
                            return false;
                        }
                    }
                    std::string function_name = scope::fully_qualified_function_name::of(n->child(1));
                    std::vector<parameter> params = new_scope->params().unpack();
                    uint32_t gi = has_paramlist ? 5 : 4;
                    if (!logic_name::of_callee(function_name,
                                               scope::type_name::of(n->child(0)),
                                               params,
                                               [n, gi, &fo](logic_writer&) -> bool {
                                                   return code_gen_of(n->child(gi)).build(fo);
                                               },
                                               fo))
                    {
                        return false;
                    }
                    std::string mangled = logic_name::of_function(function_name, params);
                    o.append(new_scope->call_hierarchy().filter(
                        mangled,
                        [fo]() -> std::string {
                            return fo.dump();
                        }));
                    return true;
                }
            };

            class delegate_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "delegate"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() >= 5);
                    parameter_type return_type = scope::normalized_type::parameter_type_of(n->child(1));
                    parameter_type name = scope::normalized_type::parameter_type_of(n->child(2));
                    std::vector<parameter_type> ps;
                    if (n->child_count() != 5)
                    {
                        for (const auto& s : GET_CODE_GENS().of_all_children(n->child(4)).dump())
                        {
                            ps.push_back(scope::normalized_type::parameter_type_of(s));
                        }
                    }
                    std::vector<std::string> param_type_strs;
                    param_type_strs.reserve(ps.size());
                    for (const auto& p : ps)
                    {
                        param_type_strs.push_back(p.full_type());
                    }
                    return scope::current()->delegates().define(name.full_type(), return_type.full_type(), ps) &&
                           builders::of_callee_ref(name.full_type(), return_type.full_type(), param_type_strs).to(o);
                }
            };

#undef GET_CODE_GENS
#undef GLUE
#undef GLUE_HELPER
