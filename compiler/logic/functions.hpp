#pragma once
#include <string>
#include <vector>
#include <memory>
#include "instruction_gen.hpp"
#include "scope.hpp"
#include "variable.hpp"
#include "control_flow.hpp"
#include "copy_move.hpp"
#include "copy_const.hpp"
#include "../../interpreter/primitive/interrupts.hpp"
#include "../../interpreter/primitive/data_block.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            using parameter = osi::compiler::parameter;
            template <typename ParamT = parameter>
            using function_signature = osi::compiler::function_signature<ParamT>;

            class _type : public instruction_gen
            {
            private:
                std::string type;
                uint32_t size{0};

            public:
                _type(std::string type, uint32_t size)
                    : type(std::move(type)), size(size)
                {
                    assert(!this->type.empty());
                }

                bool build(std::vector<std::string>&) const override
                {
                    return scope::current()->types().define(type, size);
                }
            };

            class _define : public instruction_gen
            {
            public:
                std::string name;
                std::string type;
                bool push{true};

                _define(std::string name, std::string type, bool push = true)
                    : name(std::move(name)), type(std::move(type)), push(push)
                {
                    assert(!this->name.empty());
                    assert(!this->type.empty());
                }

                static bool export_def(const std::string& name, const std::string& type, std::vector<std::string>& o)
                {
                    return _define(name, type, true).build(o);
                }

                static bool forward(const std::string& name, const std::string& type, std::vector<std::string>& o)
                {
                    return _define(name, type, false).build(o);
                }

                bool define_variable(const std::string& t, std::vector<std::string>& o) const
                {
                    uint32_t sz = 0;
                    if (!scope::current()->types().retrieve(t, sz)) return false;
                    if (!scope::current()->variables().define(name, t)) return false;
                    if (push)
                    {
                        o.push_back("push");
                        scope::current()->when_end_scope([&o]()
                        {
                            o.push_back("pop");
                        });
                    }
                    return true;
                }

                bool define_callee_ref(std::vector<std::string>& o) const
                {
                    return define_variable(scope::type_t::ptr_type, o) &&
                           scope::current()->anchor_refs().define(type, name);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    if (define_variable(type, o) || define_callee_ref(o)) return true;
                    errors::type_undefined(type, name);
                    return false;
                }
            };

            class _define_heap : public instruction_gen
            {
            private:
                std::string name;
                std::string type;
                std::string size_var_name;

            public:
                _define_heap(std::string name, std::string type, std::string size_var_name)
                    : name(std::move(name)), type(std::move(type)), size_var_name(std::move(size_var_name))
                {
                    assert(!this->name.empty());
                    assert(!this->type.empty());
                    assert(!this->size_var_name.empty());
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable sz;
                    if (!variable::of(size_var_name, o, sz)) return false;
                    if (!scope::current()->variables().define(name, type)) return false;
                    o.push_back("push");
                    o.push_back(strcat("alloc rel0 ", sz.ToString()));
                    std::string var_name = name;
                    scope::current()->when_end_scope([&o, var_name]()
                    {
                        if (scope::current()->variables().defined_in_current_scope(var_name))
                        {
                            o.push_back("dealloc rel0");
                        }
                        o.push_back("pop");
                    });
                    return true;
                }
            };

            class _dealloc_heap : public instruction_gen
            {
            private:
                std::string name;

            public:
                explicit _dealloc_heap(std::string name) : name(std::move(name))
                {
                    assert(!this->name.empty());
                }

                bool build(std::vector<std::string>& o) const override
                {
                    if (variable::is_heap_name(name))
                    {
                        errors::not_a_stack_var(name);
                        return false;
                    }
                    variable ptr;
                    if (!variable::of(name, nullptr, ptr)) return false;
                    o.push_back(strcat("dealloc ", ptr.ToString()));
                    return true;
                }
            };

            class _redefine : public instruction_gen
            {
            private:
                std::string name;
                std::string type;

            public:
                _redefine(std::string name, std::string type)
                    : name(std::move(name)), type(std::move(type))
                {
                    assert(!this->name.empty());
                    assert(!this->type.empty());
                }

                bool build(std::vector<std::string>&) const override
                {
                    if (variable::is_heap_name(name))
                    {
                        errors::not_a_stack_var(name);
                        return false;
                    }
                    variable ptr;
                    if (!variable::of(name, nullptr, ptr)) return false;
                    return scope::current()->variables().redefine(ptr.name, type);
                }
            };

            class _undefine : public instruction_gen
            {
            private:
                std::string name;

            public:
                explicit _undefine(std::string name) : name(std::move(name))
                {
                    assert(!this->name.empty());
                }

                bool build(std::vector<std::string>&) const override
                {
                    if (variable::is_heap_name(name))
                    {
                        errors::not_a_stack_var(name);
                        return false;
                    }
                    variable ptr;
                    if (!variable::of(name, nullptr, ptr)) return false;
                    return scope::current()->variables().undefine(ptr.name);
                }
            };

            class return_value
            {
            public:
                static std::string variable_name(const std::string& name)
                {
                    return "@return_value_of_" + name + "_place_holder";
                }

                static bool define(const std::string& name, const std::string& type)
                {
                    return scope::current()->variables().define(variable_name(name), type);
                }

                static bool export_slot(const std::string& name, const std::string& type, std::vector<std::string>& o)
                {
                    return _define::export_def(variable_name(name), type, o);
                }

                static bool retrieve(const std::string& name, std::vector<std::string>* v, variable& o)
                {
                    scope::exported_ref r;
                    std::string target_name = name;
                    if (!scope::current()->variables().export_var(variable_name(target_name), r))
                    {
                        if (name.rfind("::", 0) != 0 && scope::current()->variables().export_var(variable_name("::" + name), r))
                        {
                            target_name = "::" + name;
                        }
                        else if (name.rfind("::", 0) == 0 && scope::current()->variables().export_var(variable_name(name.substr(2)), r))
                        {
                            target_name = name.substr(2);
                        }
                    }
                    return variable::of(variable_name(target_name), v, o);
                }
            };

            class _callee : public instruction_gen
            {
            public:
                std::string name;
                std::string type;
                std::vector<parameter> parameters;
                std::shared_ptr<paragraph> p;

                _callee(std::string name, std::string type, std::shared_ptr<paragraph> p, std::vector<parameter> parameters = {})
                    : name(std::move(name)), type(std::move(type)), parameters(std::move(parameters)), p(std::move(p))
                {
                    assert(!this->name.empty());
                    assert(!this->type.empty());
                    assert(this->p != nullptr);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    uint32_t pos = static_cast<uint32_t>(o.size());
                    o.push_back(""); // placeholder for jump over callee

                    if (!scope::current()->anchors().define(name, static_cast<uint32_t>(o.size()), type, parameters))
                    {
                        return false;
                    }

                    {
                        scope child_scope(scope::current());
                        if (!return_value::define(name, type)) return false;
                        for (const auto& param : parameters)
                        {
                            if (!_define::forward(param.name, param.unrefed_type(), o)) return false;
                        }
                        if (!p->build(o)) return false;
                        o.push_back("rest");
                        o[pos] = strcat("jump rel", o.size() - pos);
                    }
                    return true;
                }
            };

            class anchor_caller : public instruction_gen
            {
            protected:
                std::string cmd;
                std::string result;
                std::vector<std::string> parameters;

                anchor_caller(std::string cmd, std::string result, std::vector<std::string> parameters)
                    : cmd(std::move(cmd)), result(std::move(result)), parameters(std::move(parameters)) {}

                virtual bool retrieve_anchor(scope::anchor& a) const = 0;
                virtual bool get_target_jump(const scope::anchor& a, std::string& target_jump) const = 0;

                static std::string parameter_place_holder_of(const scope::anchor& a, size_t i)
                {
                    return "@parameter_" + std::to_string(i) + "_of_" + a.name + "_place_holder";
                }

            public:
                bool build(std::vector<std::string>& o) const override
                {
                    scope child_scope(scope::current());
                    scope::anchor a;
                    if (!retrieve_anchor(a)) return false;

                    if (!return_value::export_slot(a.name, a.return_type, o)) return false;

                    if (parameters.size() != a.parameters.size())
                    {
                        errors::raise("Parameters count does not match callee ", a.name);
                        return false;
                    }

                    std::vector<variable> vars(parameters.size());
                    for (size_t i = 0; i < parameters.size(); ++i)
                    {
                        if (!variable::of(parameters[i], o, vars[i])) return false;
                    }

                    for (size_t i = 0; i < parameters.size(); ++i)
                    {
                        std::string placeholder = parameter_place_holder_of(a, i);
                        if (!_define::export_def(placeholder, a.parameters[i].unrefed_type(), o)) return false;
                        variable target;
                        if (!variable::of(placeholder, o, target)) return false;
                        if (a.parameters[i].ref)
                        {
                            if (!_move::export_instruction(target, vars[i], o)) return false;
                        }
                        else
                        {
                            if (!_copy::export_instruction(target, vars[i], o)) return false;
                        }
                    }

                    std::string target_jump;
                    if (!get_target_jump(a, target_jump)) return false;

                    o.push_back("stst");
                    o.push_back(strcat(cmd, " ", target_jump));

                    for (size_t i = 0; i < parameters.size(); ++i)
                    {
                        if (!a.parameters[i].ref) continue;
                        variable placeholder;
                        assert(variable::of(parameter_place_holder_of(a, i), o, placeholder));
                        variable param;
                        assert(variable::of(parameters[i], o, param));
                        if (!_move::export_instruction(param, placeholder, o)) return false;
                    }

                    if (!result.empty())
                    {
                        variable result_var;
                        if (!variable::of(result, o, result_var))
                        {
                            errors::variable_undefined(result);
                            return false;
                        }
                        variable ret_val;
                        assert(return_value::retrieve(a.name, &o, ret_val));
                        return _move::export_instruction(result_var, ret_val, o);
                    }
                    return true;
                }
            };

            class _caller : public anchor_caller
            {
            private:
                std::string name;

            public:
                _caller(std::string name, std::string result, std::vector<std::string> parameters)
                    : anchor_caller("jump", std::move(result), std::move(parameters)), name(std::move(name))
                {
                    assert(!this->name.empty());
                }

                _caller(std::string name, std::vector<std::string> parameters)
                    : _caller(std::move(name), "", std::move(parameters)) {}

                template <typename... Args>
                _caller(std::string name, std::string result, Args&&... args)
                    : _caller(std::move(name), std::move(result), std::vector<std::string>{std::forward<Args>(args)...}) {}

                bool retrieve_anchor(scope::anchor& a) const override
                {
                    if (scope::current()->anchors().of(name, a))
                    {
                        return true;
                    }
                    errors::anchor_undefined(name);
                    return false;
                }

                bool get_target_jump(const scope::anchor& a, std::string& target_jump) const override
                {
                    target_jump = "abs" + std::to_string(a.begin);
                    return true;
                }
            };

            class _callee_ref : public instruction_gen
            {
            private:
                std::string name;
                std::string return_type;
                std::vector<parameter_type> parameters;

            public:
                _callee_ref(std::string name, std::string return_type, std::vector<parameter_type> parameters = {})
                    : name(std::move(name)), return_type(std::move(return_type)), parameters(std::move(parameters))
                {
                    assert(!this->name.empty());
                    assert(!this->return_type.empty());
                }

                bool build(std::vector<std::string>&) const override
                {
                    return scope::current()->anchor_refs().decl(name, return_type, parameters);
                }
            };

            class _caller_ref : public anchor_caller
            {
            private:
                std::string name;

            public:
                _caller_ref(std::string name, std::string result, std::vector<std::string> parameters)
                    : anchor_caller("jmpr", std::move(result), std::move(parameters)), name(std::move(name))
                {
                    assert(!this->name.empty());
                }

                _caller_ref(std::string name, std::vector<std::string> parameters)
                    : _caller_ref(std::move(name), "", std::move(parameters)) {}

                bool retrieve_anchor(scope::anchor& a) const override
                {
                    scope::anchor_ref ar;
                    if (!scope::current()->anchor_refs().of(name, ar))
                    {
                        errors::anchor_ref_undefined(name);
                        return false;
                    }
                    std::vector<parameter> params;
                    for (size_t i = 0; i < ar.parameters.size(); ++i)
                    {
                        params.emplace_back(ar.parameters[i].unrefed_type(), ar.parameters[i].ref, "@param_" + std::to_string(i));
                    }
                    a = scope::anchor(name, 0, ar.return_type, params);
                    return true;
                }

                bool get_target_jump(const scope::anchor&, std::string& target_jump) const override
                {
                    scope::exported_ref exp;
                    if (!scope::current()->variables().export_var(name, exp))
                    {
                        errors::anchor_ref_undefined(name);
                        return false;
                    }
                    target_jump = exp.data_ref.as_string();
                    return true;
                }
            };

            class _address_of : public instruction_gen
            {
            private:
                std::string target;
                std::string name;

            public:
                _address_of(std::string target, std::string name)
                    : target(std::move(target)), name(std::move(name))
                {
                    assert(!this->target.empty());
                    assert(!this->name.empty());
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable t;
                    if (!variable::of(target, o, t) || !t.is_assignable_from_uint32()) return false;
                    scope::anchor a;
                    if (!scope::current()->anchors().of(name, a))
                    {
                        errors::anchor_undefined(name);
                        return false;
                    }
                    o.push_back(strcat("cpc ", t.ToString(), " i", a.begin));
                    return true;
                }
            };

            class _return : public instruction_gen
            {
            private:
                std::string name;
                std::string ret_var_name;

            public:
                _return(std::string name, std::string ret_var_name = "")
                    : name(std::move(name)), ret_var_name(std::move(ret_var_name))
                {
                    assert(!this->name.empty());
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable r;
                    if (!return_value::retrieve(name, &o, r)) return false;
                    if (!ret_var_name.empty() && ret_var_name != "*")
                    {
                        variable var;
                        if (!variable::of(ret_var_name, o, var)) return false;
                        if (!_copy::export_instruction(r, var, o)) return false;
                    }
                    else
                    {
                        if (!scope::type_t::is_zero_size(r.size))
                        {
                            errors::raise("target ", r.name, " [", r.type, "] as return value is expected but not provided.");
                            return false;
                        }
                    }
                    o.push_back("rest");
                    return true;
                }
            };

            class _interrupt : public instruction_gen
            {
            private:
                std::string function_name;
                std::string parameter;
                std::string result;

            public:
                _interrupt(std::string function_name, std::string parameter, std::string result)
                    : function_name(std::move(function_name)), parameter(std::move(parameter)), result(std::move(result))
                {
                    assert(!this->function_name.empty());
                    assert(!this->parameter.empty());
                    assert(!this->result.empty());
                }

                bool build(std::vector<std::string>& o) const override
                {
                    uint32_t function_id = 0;
                    if (!scope::current()->functions().of(function_name, function_id))
                    {
                        errors::interrupt_undefined(function_name);
                        return false;
                    }

                    _copy_const cpc(result, primitive::data_block::from_int32(static_cast<int32_t>(function_id)));
                    if (!cpc.build(o)) return false;

                    variable p, r;
                    if (!variable::of(parameter, o, p)) return false;
                    assert(variable::of(result, o, r));

                    o.push_back(strcat("int ", r.ToString(), " ", p.ToString(), " ", r.ToString()));
                    return true;
                }
            };

            class _stop : public instruction_gen
            {
            public:
                bool build(std::vector<std::string>& o) const override
                {
                    o.push_back("stop");
                    return true;
                }
            };
        }
    }
}
