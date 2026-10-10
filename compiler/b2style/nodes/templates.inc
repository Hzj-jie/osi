#ifndef DIALECT
#error "DIALECT must be defined before including templates.inc"
#endif
#ifndef WRITER
#error "WRITER must be defined before including templates.inc"
#endif
#ifndef TEMPLATE_TYPE_FORMAT
#error "TEMPLATE_TYPE_FORMAT must be defined before including templates.inc"
#endif
#ifndef FUNCTION_CALL_BUILD
#error "FUNCTION_CALL_BUILD must be defined before including templates.inc"
#endif

namespace osi
{
    namespace compiler
    {
        namespace DIALECT
        {
            class template_node : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "template"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER&) override
                {
                    std::string name;
                    std::shared_ptr<template_template> t;
                    return template_t::of(n, name, t) &&
                           scope::current()->template_table().define(name, t);
                }
            };

            class template_type_name : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "template-type-name"; }

                static std::vector<std::string> extract_param_types(const std::shared_ptr<automata::typed_node>& paramtypelist)
                {
                    std::vector<std::string> types;
                    for (uint32_t i = 0; i < paramtypelist->child_count(); ++i)
                    {
                        auto p = paramtypelist->child(i);
                        if (p->type_name == "type-param-with-comma" ||
                            p->type_name == "paramtype-with-comma" ||
                            p->type_name == "type-name-with-comma")
                        {
                            p = p->child(0);
                        }
                        std::string s;
                        if (code_gen_of(p).dump(s))
                        {
                            while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.pop_back();
                            while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
                            types.push_back(s);
                        }
                        else
                        {
                            types.push_back(p->input_without_ignored());
                        }
                    }
                    return types;
                }

                static bool resolve(const std::shared_ptr<automata::typed_node>& n,
                                    std::string& extended_type_name)
                {
                    assert(n != nullptr && n->child_count() == 4);
                    std::string raw_name = n->child(0)->input_without_ignored();
                    size_t dot_pos = raw_name.rfind('.');
                    std::string lookup_base = (dot_pos != std::string::npos) ? raw_name.substr(dot_pos + 1) : raw_name;
                    auto types = extract_param_types(n->child(2));
                    std::string lookup_name = template_t::name_of(lookup_base, static_cast<uint32_t>(types.size()));
                    int res = scope::current()->template_table().resolve(lookup_name, types, extended_type_name);
                    if (res <= 0)
                    {
                        raise_error("Cannot resolve template ", lookup_name);
                        return false;
                    }
                    return true;
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER& o) override
                {
                    assert(n != nullptr);
                    std::string extended_type;
                    if (!resolve(n, extended_type))
                    {
                        return false;
                    }
                    return o.append(TEMPLATE_TYPE_FORMAT(extended_type));
                }
            };

            class function_call_with_template : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "function-call-with-template"; }

                static bool split_struct_function(const std::string& name, std::string& obj, std::string& func)
                {
                    size_t dot_pos = name.rfind('.');
                    if (dot_pos == std::string::npos || dot_pos == 0 || dot_pos + 1 >= name.length())
                    {
                        return false;
                    }
                    obj = name.substr(0, dot_pos);
                    func = name.substr(dot_pos + 1);
                    return true;
                }

                static bool build(
                    const std::shared_ptr<automata::typed_node>& n,
                    WRITER& o,
                    const std::function<bool(const std::string&, const std::shared_ptr<automata::typed_node>&, WRITER&)>& function_call_build)
                {
                    assert(n != nullptr);
                    assert(function_call_build != nullptr);
                    std::string raw_name = scope::function_name::of(n->child(0)->child(0));
                    std::string obj, func;
                    bool has_dot = split_struct_function(raw_name, obj, func);

                    std::string extended_type;
                    if (!template_type_name::resolve(n->child(0), extended_type))
                    {
                        return false;
                    }
                    std::string clean_extended_type = extended_type;
                    while (clean_extended_type.rfind(current_namespace_t::namespace_separator, 0) == 0)
                    {
                        clean_extended_type = clean_extended_type.substr(current_namespace_t::namespace_separator.length());
                    }
                    std::string call_target = has_dot ? (obj + "." + clean_extended_type) : extended_type;
                    return function_call_build(call_target, n, o);
                }

#ifdef FUNCTION_CALL_WITHOUT_RETURN
                static bool without_return(const std::shared_ptr<automata::typed_node>& n, WRITER& o)
                {
                    return build(n, o, [](const std::string& fn, const std::shared_ptr<automata::typed_node>& node, WRITER& oo) {
                        return FUNCTION_CALL_WITHOUT_RETURN(fn, node, oo);
                    });
                }
#endif

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER& o) override
                {
                    return build(n, o, [](const std::string& fn, const std::shared_ptr<automata::typed_node>& node, WRITER& oo) {
                        return FUNCTION_CALL_BUILD(fn, node, oo);
                    });
                }

                static bool name_of(const std::shared_ptr<automata::typed_node>& n, std::string& o)
                {
                    assert(n != nullptr && n->child_count() == 4);
                    std::string raw_name = scope::function_name::of(n->child(0));
                    std::string obj, func;
                    std::string function_name = split_struct_function(raw_name, obj, func) ? func : raw_name;
                    o = scope::template_t::name_of(function_name, n->child(2)->child_count());
                    return true;
                }
            };

            class function_name_with_template
            {
            public:
                static const char* node_name() { return "function-name-with-template"; }

                static bool name_of(const std::shared_ptr<automata::typed_node>& n, std::string& o)
                {
                    return function_call_with_template::name_of(n, o);
                }
            };

            class delegate_with_semi_colon_node : public code_gen<WRITER>
            {
            public:
                static const char* node_name() { return "delegate-with-semi-colon"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, WRITER& o) override
                {
                    assert(n != nullptr && n->child_count() >= 1);
                    return code_gen_of(n->child(0)).build(o);
                }

                static bool name_node_of(const std::shared_ptr<automata::typed_node>& n, std::shared_ptr<automata::typed_node>& o)
                {
                    assert(n != nullptr);
                    o = n->child(0)->child(2);
                    return true;
                }

                static bool name_of(const std::shared_ptr<automata::typed_node>& n, std::string& o)
                {
                    assert(n != nullptr);
                    std::shared_ptr<automata::typed_node> name_node;
                    name_node_of(n, name_node);
                    o = scope::template_t::name_of(name_node->input_without_ignored(),
                                                   scope::template_t::type_param_count(n));
                    return true;
                }
            };
        }
    }
}

#undef DIALECT
#undef WRITER
#undef TEMPLATE_TYPE_FORMAT
#undef FUNCTION_CALL_BUILD
#ifdef FUNCTION_CALL_WITHOUT_RETURN
#undef FUNCTION_CALL_WITHOUT_RETURN
#endif
