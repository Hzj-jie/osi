#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../b3style_forward.hpp"
#include "struct_and_values.hpp"
#include "functions.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class class_initializer_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "class-initializer"; }

                static std::string failed_to_build_destructor_message(const std::string& name)
                {
                    assert(!name.empty());
                    return "Failed to build the destructor of the variable " + name +
                           ", this shouldn't happen unless the reinterpret_cast is used which changed the definition of the variable.";
                }

                static std::string failed_to_build_constructor_message(const std::string& class_name,
                                                                      const std::string& var_name)
                {
                    assert(!class_name.empty());
                    assert(!var_name.empty());
                    return class_name + " is not a class or struct, and the variable " +
                           var_name + " cannot be initialized as a class variable.";
                }

                static bool construct(const std::string& name, logic_writer& o)
                {
                    assert(!name.empty());
                    if (!function_call_node::ignore_parameters::without_return(
                            name + "." + scope::class_def::construct, o))
                    {
                        return false;
                    }
                    scope::current()->call_hierarchy().to(
                        namespace_t::fully_qualified_name(scope::class_def::construct));
                    return true;
                }

                static void destruct(const std::string& name, logic_writer& o)
                {
                    scope::current()->when_end_scope([name, &o]() {
                        value_list::with_empty();
                        if (!function_call_node::ignore_parameters::without_return(
                                name + "." + scope::class_def::destruct, o))
                        {
                            raise_error(failed_to_build_destructor_message(name));
                        }
                    });
                    scope::current()->call_hierarchy().to(
                        namespace_t::fully_qualified_name(scope::class_def::destruct));
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && (n->child_count() == 4 || n->child_count() == 5));
                    if (!struct_node::define_in_stack(n->child(0), n->child(1), o))
                    {
                        raise_error(failed_to_build_constructor_message(n->child(0)->input(), n->child(1)->input()));
                        return false;
                    }
                    if (n->child_count() == 4)
                    {
                        value_list::with_empty();
                    }
                    else if (!code_gen_of(n->child(3)).build(o))
                    {
                        return false;
                    }
                    std::string name = scope::fully_qualified_variable_name::of(n->child(1));
                    if (construct(name, o))
                    {
                        destruct(name, o);
                        return true;
                    }
                    return false;
                }
            };

            inline bool value_declaration::build(const std::shared_ptr<automata::typed_node>& n,
                                                 bool class_construct,
                                                 logic_writer& o)
            {
                assert(n != nullptr && n->child_count() >= 2);
                std::string type = scope::normalized_type::of(n->child(0));
                std::string name = scope::fully_qualified_variable_name::of(n->child(1));
                if (!struct_node::define_in_stack(n->child(0), n->child(1), o))
                {
                    return declare_primitive_type(type, name, o);
                }
                if (scope::current()->classes().is_defined(type))
                {
                    if (class_construct)
                    {
                        value_list::with_empty();
                        if (!class_initializer_node::construct(name, o))
                        {
                            return false;
                        }
                    }
                    class_initializer_node::destruct(name, o);
                }
                return true;
            }

            class class_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "class"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr && n->child_count() >= 5);

                    std::string class_name = n->child(1)->input_without_ignored();
                    class_def cd(class_name);

                    bool has_constructor = false;
                    bool has_destructor = false;

                    uint32_t body_start = 3;
                    if (n->child(2)->type_name == "class-inheritance")
                    {
                        body_start = 4;
                    }

                    for (uint32_t i = body_start; i + 2 < n->child_count(); ++i)
                    {
                        auto child = n->child(i);
                        if (child->type_name == "struct-body")
                        {
                            if (child->child_count() >= 2 && child->child(0)->type_name == "value-declaration")
                            {
                                auto vd = child->child(0);
                                std::string t = scope::normalized_type::of(vd->child(0));
                                std::string v = vd->child(1)->input_without_ignored();
                                while (v.rfind(current_namespace_t::namespace_separator, 0) == 0)
                                {
                                    v = v.substr(current_namespace_t::namespace_separator.length());
                                }
                                cd.with_var(parameter::non_ref(t, v));
                            }
                        }
                        else if (child->type_name == "class-function")
                        {
                            auto fn_node = child->child(0);
                            class_def::function_def::type_t f_type = class_def::function_def::type_t::pure;
                            if (fn_node->type_name == "overridable-function")
                            {
                                f_type = class_def::function_def::type_t::overridable;
                                fn_node = fn_node->child(1);
                            }
                            else if (fn_node->type_name == "override-function")
                            {
                                f_type = class_def::function_def::type_t::override;
                                fn_node = fn_node->child(1);
                            }

                            std::string ret_type = fn_node->child(0)->input_without_ignored();
                            std::string fn_name = fn_node->child(1)->input_without_ignored();
                            if (fn_name == class_def::construct) has_constructor = true;
                            if (fn_name == class_def::destruct) has_destructor = true;

                            std::vector<std::pair<std::string, std::string>> params;
                            std::vector<std::string> param_names;
                            if (fn_node->child_count() == 6)
                            {
                                auto paramlist = fn_node->child(3);
                                for (uint32_t p = 0; p < paramlist->child_count(); ++p)
                                {
                                    auto pnode = paramlist->child(p);
                                    if (pnode->type_name == "param-with-comma")
                                    {
                                        pnode = pnode->child(0);
                                    }
                                    assert(pnode->type_name == "param");
                                    bool is_ref = (pnode->child_count() == 3);
                                    std::string pt = pnode->child(0)->input_without_ignored() + (is_ref ? "&" : "");
                                    std::string pn = pnode->last_child()->input_without_ignored();
                                    params.emplace_back(pt, pn);
                                    param_names.push_back(pn);
                                }
                            }

                            class_def::function_def fdef;
                            fdef.return_type = ret_type;
                            fdef.name = fn_name;
                            fdef.params = params;
                            fdef.type = f_type;
                            fdef.content = fdef.declaration(cd.name.fully_qualified_name(), param_names) + fn_node->last_child()->input();
                            cd.funcs_.push_back(std::move(fdef));
                        }
                        else if (child->type_name == "class-template-function")
                        {
                            auto head = child->child(0);
                            auto fn_wrapper = child->child(1);
                            auto fn_node = fn_wrapper->child(0);
                            class_def::function_def::type_t f_type = class_def::function_def::type_t::pure;
                            if (fn_node->type_name == "overridable-function")
                            {
                                f_type = class_def::function_def::type_t::overridable;
                                fn_node = fn_node->child(1);
                            }
                            else if (fn_node->type_name == "override-function")
                            {
                                f_type = class_def::function_def::type_t::override;
                                fn_node = fn_node->child(1);
                            }

                            std::string ret_type = fn_node->child(0)->input_without_ignored();
                            std::string fn_name = fn_node->child(1)->input_without_ignored();

                            std::vector<std::pair<std::string, std::string>> params;
                            std::vector<std::string> param_names;
                            if (fn_node->child_count() == 6)
                            {
                                auto paramlist = fn_node->child(3);
                                for (uint32_t p = 0; p < paramlist->child_count(); ++p)
                                {
                                    auto pnode = paramlist->child(p);
                                    if (pnode->type_name == "param-with-comma")
                                    {
                                        pnode = pnode->child(0);
                                    }
                                    assert(pnode->type_name == "param");
                                    bool is_ref = (pnode->child_count() == 3);
                                    std::string pt = pnode->child(0)->input_without_ignored() + (is_ref ? "&" : "");
                                    std::string pn = pnode->last_child()->input_without_ignored();
                                    params.emplace_back(pt, pn);
                                    param_names.push_back(pn);
                                }
                            }

                            std::unordered_set<std::string> tmpl_type_names;
                            auto tmpl_param_list = head->child(2);
                            for (uint32_t tp = 0; tp < tmpl_param_list->child_count(); ++tp)
                            {
                                auto tpnode = tmpl_param_list->child(tp);
                                if (tpnode->type_name == "type-param-with-comma")
                                {
                                    tpnode = tpnode->child(0);
                                }
                                tmpl_type_names.insert(tpnode->input_without_ignored());
                            }

                            class_def::function_def fdef;
                            fdef.return_type = ret_type;
                            fdef.name = fn_name;
                            fdef.params = params;
                            fdef.type = f_type;
                            fdef.content = fdef.declaration(cd.name.fully_qualified_name(), param_names, tmpl_type_names) + fn_node->last_child()->input();
                            cd.temps_.emplace_back(head->input(), std::move(fdef));
                        }
                    }

                    if (!has_constructor)
                    {
                        class_def::function_def cdef;
                        cdef.return_type = "::void";
                        cdef.name = class_def::construct;
                        cdef.type = class_def::function_def::type_t::pure;
                        cdef.content = "::void " + namespace_t::fully_qualified_name(class_def::construct) + "(" +
                                       cd.name.fully_qualified_name() + "& this){}";
                        cd.funcs_.push_back(std::move(cdef));
                    }
                    if (!has_destructor)
                    {
                        class_def::function_def ddef;
                        ddef.return_type = "::void";
                        ddef.name = class_def::destruct;
                        ddef.type = class_def::function_def::type_t::pure;
                        ddef.content = "::void " + namespace_t::fully_qualified_name(class_def::destruct) + "(" +
                                       cd.name.fully_qualified_name() + "& this){}";
                        cd.funcs_.push_back(std::move(ddef));
                    }

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
                            std::string clean_base_type = base_type;
                            while (clean_base_type.rfind(current_namespace_t::namespace_separator, 0) == 0)
                            {
                                clean_base_type = clean_base_type.substr(current_namespace_t::namespace_separator.length());
                            }
                            cd.with_var(parameter::non_ref(base_type + "__struct__type__id__type", clean_base_type + "__struct__type__id"));
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
