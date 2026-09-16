#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <functional>
#include <algorithm>
#include <sstream>
#include "../scope/scope.hpp"
#include "../scope/struct_def.hpp"
#include "../function_signature.hpp"
#include "../read_scoped.hpp"
#include "../logic/logic_writer.hpp"
#include "../logic/builders.hpp"
#include "../logic/scope.hpp"
#include "../logic/variable.hpp"
#include "../code_gen/code_gen.hpp"
#include "../code_gen/statements.hpp"
#include "../../automata/typed_node.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"
#include "b3style_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class scope;

            inline thread_local bool _disable_namespace = false;

            class disable_namespace_guard
            {
            private:
                bool prev_;
            public:
                disable_namespace_guard()
                {
                    assert(!_disable_namespace);
                    prev_ = _disable_namespace;
                    _disable_namespace = true;
                }
                ~disable_namespace_guard()
                {
                    _disable_namespace = prev_;
                }
            };

            inline disable_namespace_guard disable_namespace()
            {
                return disable_namespace_guard();
            }

            inline bool with_namespace()
            {
                return !_disable_namespace;
            }

            class current_namespace_t
            {
            private:
                std::vector<std::string> s_;

            public:
                inline static const std::string namespace_separator = "::";

                struct defer_pop
                {
                    current_namespace_t* self{nullptr};
                    defer_pop() = default;
                    explicit defer_pop(current_namespace_t* s) : self(s) {}
                    defer_pop(const defer_pop&) = delete;
                    defer_pop& operator=(const defer_pop&) = delete;
                    defer_pop(defer_pop&& o) noexcept : self(o.self) { o.self = nullptr; }
                    defer_pop& operator=(defer_pop&& o) noexcept
                    {
                        if (this != &o)
                        {
                            if (self && !self->s_.empty()) self->s_.pop_back();
                            self = o.self;
                            o.self = nullptr;
                        }
                        return *this;
                    }
                    ~defer_pop()
                    {
                        if (self && !self->s_.empty())
                        {
                            self->s_.pop_back();
                        }
                    }
                };

                std::shared_ptr<defer_pop> define(const std::string& name);

                static std::string of(const std::string& i);

                static std::pair<std::string, std::string> of_namespace_and_name(const std::string& i);

                const std::vector<std::string>& stack() const { return s_; }
                std::vector<std::string>& stack() { return s_; }
            };

            class namespace_t
            {
            public:
                static std::string fully_qualified_name(const std::string& n)
                {
                    assert(!n.empty());
                    if (!with_namespace())
                    {
                        return n;
                    }
                    if (n.rfind(current_namespace_t::namespace_separator, 0) == 0)
                    {
                        return n;
                    }
                    return current_namespace_t::namespace_separator + n;
                }

                static std::string fully_qualified_name(const std::string& n, const std::string& i)
                {
                    assert(!i.empty());
                    if (!with_namespace())
                    {
                        return i;
                    }
                    if (i.rfind(current_namespace_t::namespace_separator, 0) == 0)
                    {
                        return i;
                    }
                    if (n.empty())
                    {
                        return fully_qualified_name(i);
                    }
                    return fully_qualified_name(n + current_namespace_t::namespace_separator + i);
                }
            };

            struct name_with_namespace
            {
                std::string ns;
                std::string name;

                static name_with_namespace of(const std::string& i)
                {
                    if (!with_namespace())
                    {
                        return {"", i};
                    }
                    auto p = current_namespace_t::of_namespace_and_name(i);
                    return {p.first, p.second};
                }

                static name_with_namespace of_global_namespace(const std::string& i)
                {
                    assert(!i.empty());
                    return {"", i};
                }

                std::string fully_qualified_name() const
                {
                    return namespace_t::fully_qualified_name(ns, name);
                }

                bool operator==(const name_with_namespace& o) const
                {
                    return ns == o.ns && name == o.name;
                }

                bool operator<(const name_with_namespace& o) const
                {
                    if (ns != o.ns) return ns < o.ns;
                    return name < o.name;
                }
            };
        }
    }
}

namespace std
{
    template <>
    struct hash<osi::compiler::b3style_compiler::name_with_namespace>
    {
        size_t operator()(const osi::compiler::b3style_compiler::name_with_namespace& n) const
        {
            return std::hash<std::string>()(n.ns) ^ (std::hash<std::string>()(n.name) << 1);
        }
    };
}

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class root_type_injector_t
            {
            private:
                std::vector<std::shared_ptr<logic_writer>> stack_;

            public:
                struct guard
                {
                    root_type_injector_t* self{nullptr};
                    guard() = default;
                    explicit guard(root_type_injector_t* s) : self(s) {}
                    ~guard()
                    {
                        if (self && !self->stack_.empty())
                        {
                            self->stack_.pop_back();
                        }
                    }
                    guard(const guard&) = delete;
                    guard& operator=(const guard&) = delete;
                    guard(guard&& o) noexcept : self(o.self) { o.self = nullptr; }
                    guard& operator=(guard&& o) noexcept
                    {
                        if (this != &o)
                        {
                            if (self && !self->stack_.empty()) self->stack_.pop_back();
                            self = o.self;
                            o.self = nullptr;
                        }
                        return *this;
                    }
                };

                [[nodiscard]] std::shared_ptr<guard> _new(logic_writer& o)
                {
                    auto w = std::make_shared<logic_writer>();
                    stack_.push_back(w);
                    o.append(w);
                    return std::make_shared<guard>(this);
                }

                std::shared_ptr<logic_writer> current()
                {
                    assert(!stack_.empty());
                    return stack_.back();
                }
            };

            class class_def;

            class scope : public scope_base<scope>
            {
            public:
                using class_def = osi::compiler::b3style_compiler::class_def;

                struct type_name
                {
                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return n->input_without_ignored();
                    }
                };

                struct variable_name
                {
                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        std::string s = n->input_without_ignored();
                        size_t pos = 0;
                        while ((pos = s.find(".::", pos)) != std::string::npos)
                        {
                            s.replace(pos, 3, ".");
                        }
                        return s;
                    }
                };

                struct function_name
                {
                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return n->input_without_ignored();
                    }
                };

                struct normalized_type
                {
                    static parameter_type parameter_type_of(const std::string& type)
                    {
                        return parameter_type::of(type).map_type([](const std::string& t) {
                            return of(t);
                        });
                    }

                    static parameter_type parameter_type_of(const std::shared_ptr<automata::typed_node>& type)
                    {
                        assert(type != nullptr);
                        if (type->type_name == "paramtype")
                        {
                            bool is_ref = (type->child_count() == 2 && type->child(1)->type_name == "reference");
                            std::string base_t = of(type->child(0));
                            return parameter_type(base_t, is_ref);
                        }
                        return parameter_type_of(of(type));
                    }

                    static std::string of(const std::string& type);

                    static std::string of(const std::shared_ptr<automata::typed_node>& type)
                    {
                        assert(type != nullptr);
                        if (type->type_name == "template-type-name" ||
                            (!type->leaf() && type->child_count() > 0 && type->child(0)->type_name == "template-type-name"))
                        {
                            std::string s;
                            auto target_node = (type->type_name == "template-type-name") ? type : type->child(0);
                            if (code_gen_of(target_node).dump(s))
                            {
                                while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\n' || s.back() == '\r')) s.pop_back();
                                while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\n' || s.front() == '\r')) s.erase(s.begin());
                                if (!s.empty())
                                {
                                    return of(s);
                                }
                            }
                        }
                        return of(type->input_without_ignored());
                    }
                };

                struct fully_qualified_variable_name
                {
                    static std::string of(const std::string& name);

                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return of(variable_name::of(n));
                    }
                };

                struct fully_qualified_function_name
                {
                    static std::string of(const std::string& name);

                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return of(function_name::of(n));
                    }
                };

                class includes_t
                {
                private:
                    std::unordered_set<std::string> included_;

                public:
                    bool should_include(const std::string& f)
                    {
                        return included_.insert(f).second;
                    }
                };

                struct includes_proxy
                {
                    bool operator()(const std::string& f) const
                    {
                        return current()->includes().should_include(f);
                    }
                };

                class define_t
                {
                private:
                    std::unordered_set<std::string> d_;

                public:
                    void define(const std::string& s)
                    {
                        d_.insert(s);
                    }

                    bool is_defined(const std::string& s) const
                    {
                        return d_.find(s) != d_.end();
                    }
                };

                class type_alias_t
                {
                private:
                    std::unordered_map<std::string, std::string> m_;

                public:
                    bool define(std::string alias, std::string canonical_type)
                    {
                        assert(!alias.empty());
                        alias = current_namespace_t::of(alias);
                        assert(!alias.empty());
                        assert(!canonical_type.empty());
                        parameter_type canonical = normalized_type::parameter_type_of(canonical_type);
                        if (parameter_type::is_ref_type(alias))
                        {
                            raise_error("Reference type ", alias, " is not allowed to be aliased. ");
                            return false;
                        }
                        if (canonical.ref)
                        {
                            raise_error("Reference type ", alias, " or canonical ", canonical.ToString(),
                                        " is not allowed to be used as a canonical type. ");
                            return false;
                        }
                        if (alias == canonical.non_ref_type())
                        {
                            raise_error("Cycle typedefs detected, alias ", alias, " equals to its canonical.");
                            return false;
                        }
                        m_[alias] = canonical.non_ref_type();
                        return true;
                    }

                    const std::unordered_map<std::string, std::string>& map() const { return m_; }

                    void remove(const std::string& alias)
                    {
                        m_.erase(alias);
                    }
                };

                class struct_t
                {
                private:
                    std::unordered_map<std::string, struct_def> s_;

                public:
                    static parameter create_type_id(const std::string& name)
                    {
                        assert(!name.empty());
                        std::string clean_name = name;
                        while (clean_name.rfind(current_namespace_t::namespace_separator, 0) == 0)
                        {
                            clean_name = clean_name.substr(current_namespace_t::namespace_separator.length());
                        }
                        return parameter::non_ref(name + "__struct__type__id__type", clean_name + "__struct__type__id");
                    }

                    bool define(const std::string& name,
                                std::vector<parameter> members,
                                const std::function<void(const std::string&, uint32_t)>& define_type)
                    {
                        assert(!name.empty());
                        std::string full_type = current_namespace_t::of(name);
                        assert(!parameter_type::is_ref_type(full_type));

                        parameter type_id = create_type_id(name);
                        if (define_type != nullptr)
                        {
                            define_type(normalized_type::of(type_id.unrefed_type()), 1);
                        }
                        members.push_back(type_id);

                        struct_def d;
                        for (const auto& m : members)
                        {
                            assert(!m.ref);
                            std::string m_type = normalized_type::parameter_type_of(m.non_ref_type()).full_type();
                            struct_def sub_def;
                            if (current()->structs_resolve(m_type, "", sub_def))
                            {
                                d.with_nested(m_type, m.name);
                                d.append(sub_def.append_prefix(m.name));
                            }
                            else
                            {
                                d.with_primitive(m_type, m.name);
                            }
                        }

                        if (s_.find(full_type) != s_.end())
                        {
                            raise_error("Struct type ", full_type, " has been defined already.");
                            return false;
                        }
                        s_[full_type] = std::move(d);
                        current()->type_alias_remove(full_type);
                        return true;
                    }

                    bool resolve(const std::string& type,
                                 const std::string& name,
                                 struct_def& o) const
                    {
                        assert(!type.empty());
                        std::string full_type = normalized_type::parameter_type_of(type).full_type();
                        auto it = s_.find(full_type);
                        if (it == s_.end() && type.rfind(current_namespace_t::namespace_separator, 0) != 0)
                        {
                            std::string global_type = current_namespace_t::namespace_separator + type;
                            it = s_.find(global_type);
                        }
                        if (it == s_.end())
                        {
                            return false;
                        }
                        o = it->second;
                        if (!name.empty())
                        {
                            std::string fq_name = fully_qualified_variable_name::of(name);
                            o = o.append_prefix(fq_name);
                        }
                        return true;
                    }
                };

                class variable_t
                {
                private:
                    std::unordered_map<std::string, parameter> s_;

                public:
                    bool define(const std::string& type, std::string name)
                    {
                        assert(!type.empty());
                        assert(!name.empty());
                        if (scope::current()->is_root())
                        {
                            name = current_namespace_t::of(name);
                        }
                        if (defined(name))
                        {
                            return false;
                        }
                        s_[name] = parameter(type, name);
                        return true;
                    }

                    bool defined(const std::string& name) const
                    {
                        parameter dummy;
                        return resolve(name, dummy);
                    }

                    bool resolve(const std::string& name, parameter& o) const
                    {
                        assert(!name.empty());
                        auto it = s_.find(name);
                        if (it != s_.end())
                        {
                            o = it->second;
                            return true;
                        }
                        std::string fn = current_namespace_t::of(name);
                        if (fn != name)
                        {
                            it = s_.find(fn);
                            if (it != s_.end())
                            {
                                o = it->second;
                                return true;
                            }
                        }
                        return false;
                    }

                    bool redefine(const std::string& type, const std::string& name)
                    {
                        assert(!type.empty());
                        assert(!name.empty());
                        auto it = s_.find(name);
                        if (it != s_.end())
                        {
                            it->second = parameter(type, name);
                            return true;
                        }
                        std::string fn = current_namespace_t::of(name);
                        if (fn != name)
                        {
                            it = s_.find(fn);
                            if (it != s_.end())
                            {
                                it->second = parameter(type, fn);
                                return true;
                            }
                        }
                        return false;
                    }

                    bool undefine(const std::string& name)
                    {
                        assert(!name.empty());
                        if (s_.erase(name) > 0) return true;
                        std::string fn = current_namespace_t::of(name);
                        if (fn != name)
                        {
                            return s_.erase(fn) > 0;
                        }
                        return false;
                    }
                };

                class function_t
                {
                private:
                    std::unordered_map<std::string, std::string> m_;

                public:
                    bool define(std::string return_type, const std::string& name)
                    {
                        assert(!return_type.empty() && !name.empty());
                        return_type = normalized_type::of(return_type);
                        auto it = m_.find(name);
                        if (it == m_.end())
                        {
                            m_[name] = return_type;
                            return true;
                        }
                        return it->second == return_type;
                    }

                    bool return_type_of(const std::string& name, std::string& return_type) const
                    {
                        auto it = m_.find(name);
                        if (it != m_.end())
                        {
                            return_type = it->second;
                            return true;
                        }
                        return false;
                    }

                    bool is_defined(const std::string& name) const
                    {
                        return m_.find(name) != m_.end();
                    }
                };

                class value_target_t
                {
                public:
                    struct target
                    {
                        std::string type;
                        std::vector<std::string> names;

                        const std::string& only() const
                        {
                            assert(names.size() == 1);
                            return names[0];
                        }
                    };

                private:
                    read_scoped<target> values_;
                    read_scoped<std::vector<std::string>> value_lists_;

                public:
                    value_target_t() = default;

                    std::vector<std::string> with_value(const std::string& type, std::vector<std::string> vs)
                    {
                        std::string t = normalized_type::parameter_type_of(type).full_type();
                        values_.push(target{t, vs});
                        return vs;
                    }

                    std::vector<std::string> with_value(const std::string& type, const std::vector<parameter>& ps)
                    {
                        std::vector<std::string> names;
                        names.reserve(ps.size());
                        for (const auto& p : ps)
                        {
                            names.push_back(p.name);
                        }
                        return with_value(type, std::move(names));
                    }

                    read_scoped<target>::ref value()
                    {
                        return values_.pop();
                    }

                    class primitive_ref
                    {
                    private:
                        read_scoped<target>::ref ref_;

                    public:
                        primitive_ref() = default;
                        explicit primitive_ref(read_scoped<target>::ref r) : ref_(std::move(r)) {}

                        bool retrieve(std::string& o) const
                        {
                            if (ref_->names.size() != 1)
                            {
                                return false;
                            }
                            o = ref_->names[0];
                            return true;
                        }
                    };

                    primitive_ref primitive_type()
                    {
                        return primitive_ref(values_.pop());
                    }

                    void with_value_list(std::vector<std::string> v)
                    {
                        value_lists_.push(std::move(v));
                    }

                    read_scoped<std::vector<std::string>>::ref value_list()
                    {
                        return value_lists_.pop();
                    }

                    std::vector<std::string> with_temp_target(const std::string& type, logic_writer& o);
                };

                class params_t
                {
                private:
                    std::vector<parameter> ps_;

                public:
                    void pack(const std::vector<parameter>& ps)
                    {
                        ps_.insert(ps_.end(), ps.begin(), ps.end());
                    }

                    std::vector<parameter> unpack()
                    {
                        std::vector<parameter> res;
                        res.swap(ps_);
                        return res;
                    }
                };

                class current_function_t
                {
                private:
                    std::string name_;
                    std::string return_type_;
                    std::vector<parameter> parameters_;

                public:
                    current_function_t() = default;

                    current_function_t(std::string n, std::string rt, std::vector<parameter> ps)
                        : name_(std::move(n)), return_type_(std::move(rt)), parameters_(std::move(ps))
                    {
                        assert(!name_.empty());
                        assert(!return_type_.empty());
                    }

                    const std::string& name() const { return name_; }
                    const std::string& return_type() const { return return_type_; }
                    const std::vector<parameter>& parameters() const { return parameters_; }

                    bool allow_return_value() const
                    {
                        return return_type_ != "void" && return_type_ != "::void" &&
                               return_type_ != "type0" && return_type_ != "::type0";
                    }

                    bool return_struct() const;

                    std::string signature() const
                    {
                        return function_signature<parameter>(name_, return_type_, parameters_).ToString();
                    }
                };

                class delegate_t
                {
                private:
                    std::unordered_map<std::string, function_signature<parameter_type>> m_;

                public:
                    bool define(const std::string& name,
                                const std::string& return_type,
                                const std::vector<parameter_type>& parameters)
                    {
                        assert(!name.empty());
                        auto it = m_.find(name);
                        if (it == m_.end())
                        {
                            m_[name] = function_signature<parameter_type>(name, return_type, parameters);
                            return true;
                        }
                        return false;
                    }

                    bool retrieve(const std::string& name, function_signature<parameter_type>& o) const
                    {
                        auto it = m_.find(name);
                        if (it != m_.end())
                        {
                            o = it->second;
                            return true;
                        }
                        return false;
                    }
                };

                class temp_logic_name_t
                {
                private:
                    uint32_t var_idx_{0};
                    uint32_t logic_idx_{0};

                public:
                    std::string variable()
                    {
                        return "@v" + std::to_string(var_idx_++);
                    }

                    std::string logic()
                    {
                        return "@l" + std::to_string(logic_idx_++);
                    }
                };

                class call_hierarchy_t
                {
                private:
                    struct state_t
                    {
                        const std::string main_name = "main";
                        std::unordered_map<std::string, std::vector<std::string>> m_;
                        std::unordered_set<std::string> tm_;

                        bool can_reach_root(const std::string& f) const
                        {
                            return tm_.find(f) != tm_.end();
                        }
                    };

                    std::shared_ptr<state_t> state_;

                public:
                    call_hierarchy_t()
                        : state_(std::make_shared<state_t>()) {}

                    void to(const std::string& to_func)
                    {
                        assert(!to_func.empty());
                        std::string from_func = state_->main_name;
                        auto* cf = current()->find_current_function();
                        if (cf != nullptr)
                        {
                            from_func = cf->name();
                        }
                        if (to_func != from_func)
                        {
                            state_->m_[from_func].push_back(to_func);
                        }
                    }

                    void calculate()
                    {
                        state_->tm_.clear();
                        std::queue<std::string> q;
                        q.push(state_->main_name);
                        state_->tm_.insert(state_->main_name);
                        while (!q.empty())
                        {
                            std::string f = q.front();
                            q.pop();
                            auto it = state_->m_.find(f);
                            if (it == state_->m_.end()) continue;
                            for (const auto& other : it->second)
                            {
                                if (state_->tm_.insert(other).second)
                                {
                                    q.push(other);
                                }
                            }
                        }
                    }

                    bool can_reach_root(const std::string& f) const
                    {
                        return state_->can_reach_root(f);
                    }

                    std::function<std::string()> filter(const std::string& f, const std::function<std::string()>& o)
                    {
                        return [st = state_, f, o]() -> std::string {
                            if (!st->can_reach_root(f))
                            {
                                return "";
                            }
                            return o();
                        };
                    }

                    class calculator : public statement<logic_writer>
                    {
                    public:
                        void export_to(logic_writer&) override
                        {
                            current()->call_hierarchy().calculate();
                        }

                        static void register_statement(statements<logic_writer>& p)
                        {
                            p.register_statement(std::make_shared<calculator>());
                        }
                    };
                };

                class class_t;
                class template_t;

            private:
                includes_t incs_;
                call_hierarchy_t fc_;
                define_t d_;
                type_alias_t ta_;
                struct_t s_;
                variable_t v_;
                function_t f_;
                value_target_t vt_;
                params_t ps_;
                current_function_t cf_;
                delegate_t de_;
                temp_logic_name_t t_;
                current_namespace_t cn_;
                std::shared_ptr<class_t> c_;
                std::shared_ptr<template_t> tt_;
                root_type_injector_t i_;

            public:
                explicit scope(scope* parent = nullptr);

                ~scope() override
                {
                    this->run_end_scope();
                }

                static scope* current()
                {
                    return scope_base<scope>::current();
                }

                includes_t& includes() { return get_root()->incs_; }
                define_t& defines() { return get_root()->d_; }
                function_t& functions() { return get_root()->f_; }
                temp_logic_name_t& temp_logic_name() { return get_root()->t_; }
                call_hierarchy_t& call_hierarchy() { return get_root()->fc_; }
                current_namespace_t& current_namespace() { return get_root()->cn_; }
                root_type_injector_t& root_type_injector() { return get_root()->i_; }
                class_t& classes() { return *get_root()->c_; }
                template_t& template_table() { return *get_root()->tt_; }

                type_alias_t& type_alias() { return get_root()->ta_; }
                struct_t& structs() { return get_root()->s_; }
                variable_t& variables() { return v_; }
                value_target_t& value_target() { return vt_; }
                params_t& params() { return ps_; }

                const current_function_t& current_function() const
                {
                    const auto* cf = find_current_function();
                    assert(cf != nullptr);
                    return *cf;
                }

                delegate_t& delegates() { return get_root()->de_; }

                std::unique_ptr<scope> start_scope()
                {
                    return std::make_unique<scope>(this);
                }

                void without_end_scope()
                {
                    disable_end_scope();
                }

                void set_current_function(const std::string& name,
                                          const std::string& return_type,
                                          const std::vector<parameter>& params)
                {
                    cf_ = current_function_t(name, return_type, params);
                }

                const current_function_t* find_current_function() const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (!s->cf_.name().empty())
                        {
                            return &s->cf_;
                        }
                        s = s->get_parent();
                    }
                    return nullptr;
                }

                std::string type_alias(std::string alias) const
                {
                    assert(!parameter_type::is_ref_type(alias));
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        auto it = s->ta_.map().find(alias);
                        if (it != s->ta_.map().end())
                        {
                            alias = it->second;
                        }
                        s = s->get_parent();
                    }
                    return alias;
                }

                void type_alias_remove(const std::string& alias)
                {
                    get_root()->ta_.remove(alias);
                }

                bool variables_redefine(const std::string& type, const std::string& name)
                {
                    scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->v_.redefine(type, name)) return true;
                        s = s->get_parent();
                    }
                    return false;
                }

                bool variables_undefine(const std::string& name)
                {
                    scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->v_.undefine(name)) return true;
                        s = s->get_parent();
                    }
                    return false;
                }

                bool variables_resolve(std::string name,
                                       parameter& o,
                                       function_signature<parameter_type>* sig = nullptr) const
                {
                    if (osi::compiler::logic::variable::is_heap_name(name))
                    {
                        name = name.substr(0, name.find('['));
                    }
                    size_t pos = 0;
                    while ((pos = name.find(".::", pos)) != std::string::npos)
                    {
                        name.replace(pos, 3, ".");
                    }
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->v_.resolve(name, o))
                        {
                            if (sig != nullptr)
                            {
                                delegates_retrieve(o.full_type(), *sig);
                            }
                            return true;
                        }
                        s = s->get_parent();
                    }
                    return false;
                }

                bool variables_defined(const std::string& name) const
                {
                    parameter dummy;
                    return variables_resolve(name, dummy);
                }

                bool variables_type_of(const std::string& name, std::string& type) const
                {
                    parameter o;
                    if (variables_resolve(name, o))
                    {
                        type = o.full_type();
                        return true;
                    }
                    return false;
                }

                bool variables_delegate_of(const std::string& name, function_signature<parameter_type>& sig) const
                {
                    parameter o;
                    if (variables_resolve(name, o, &sig))
                    {
                        return delegates_retrieve(o.full_type(), sig);
                    }
                    return false;
                }

                bool variables_define(const std::vector<parameter>& vs)
                {
                    for (const auto& p : vs)
                    {
                        if (!v_.define(p.unrefed_type(), p.name)) return false;
                    }
                    return true;
                }

                bool structs_resolve(const std::string& type, const std::string& name, struct_def& o) const
                {
                    return get_root()->s_.resolve(type, name, o);
                }

                bool structs_is_type_defined(const std::string& type) const
                {
                    struct_def dummy;
                    return structs_resolve(type, "", dummy);
                }

                bool structs_resolve_variable(const std::string& name, struct_def& o) const
                {
                    parameter p;
                    if (!variables_resolve(name, p)) return false;
                    return structs_resolve(p.full_type(), p.name, o);
                }

                bool structs_is_variable_defined(const std::string& name) const
                {
                    struct_def dummy;
                    return structs_resolve_variable(name, dummy);
                }

                bool delegates_retrieve(const std::string& name, function_signature<parameter_type>& o) const
                {
                    return get_root()->de_.retrieve(name, o);
                }
            };

            inline std::string current_namespace_t::of(const std::string& i)
            {
                assert(!i.empty());
                if (!with_namespace())
                {
                    return i;
                }
                if (i.rfind(namespace_separator, 0) == 0)
                {
                    return i;
                }
                std::string n;
                auto* cur = scope::current();
                if (cur)
                {
                    const auto& stk = cur->current_namespace().stack();
                    if (!stk.empty())
                    {
                        n = stk.back();
                    }
                }
                assert(n.empty() || n.rfind(namespace_separator, 0) == 0);
                return n + namespace_separator + i;
            }

            inline std::shared_ptr<current_namespace_t::defer_pop> current_namespace_t::define(const std::string& name)
            {
                assert(!name.empty());
                assert(with_namespace());
                assert(name.rfind(namespace_separator, 0) == 0);
                s_.push_back(name);
                return std::make_shared<defer_pop>(this);
            }

            inline std::pair<std::string, std::string> current_namespace_t::of_namespace_and_name(const std::string& i)
            {
                assert(with_namespace());
                std::string f = of(i);
                size_t index = f.rfind(namespace_separator);
                if (index == std::string::npos)
                {
                    return {"", f};
                }
                return {f.substr(0, index), f.substr(index + namespace_separator.length())};
            }

            inline std::string scope::normalized_type::of(const std::string& type)
            {
                if (type == logic::scope::type_t::variable_type ||
                    type == logic::scope::type_t::ptr_type ||
                    type == logic::scope::type_t::zero_type ||
                    type == current_namespace_t::namespace_separator + logic::scope::type_t::variable_type ||
                    type == current_namespace_t::namespace_separator + logic::scope::type_t::ptr_type ||
                    type == current_namespace_t::namespace_separator + logic::scope::type_t::zero_type)
                {
                    return type;
                }
                std::string scoped = current_namespace_t::of(type);
                std::string aliased = scope::current()->type_alias(scoped);
                if (aliased == scoped && type.rfind(current_namespace_t::namespace_separator, 0) != 0)
                {
                    std::string global_scoped = current_namespace_t::namespace_separator + type;
                    std::string global_aliased = scope::current()->type_alias(global_scoped);
                    if (global_aliased != global_scoped)
                    {
                        return global_aliased;
                    }
                }
                return aliased;
            }

            inline std::string scope::fully_qualified_variable_name::of(const std::string& name)
            {
                assert(!name.empty());
                std::string s = name;
                size_t pos = 0;
                while ((pos = s.find(".::", pos)) != std::string::npos)
                {
                    s.replace(pos, 3, ".");
                }
                if (scope::current()->is_root())
                {
                    return current_namespace_t::of(s);
                }
                return s;
            }

            inline std::string scope::fully_qualified_function_name::of(const std::string& name)
            {
                assert(!name.empty());
                if (name == "main")
                {
                    return name;
                }
                return current_namespace_t::of(name);
            }

            inline bool scope::current_function_t::return_struct() const
            {
                return scope::current()->structs_is_type_defined(return_type());
            }

            class class_def
            {
            public:
                inline static const std::string construct = "construct";
                inline static const std::string destruct = "destruct";

                struct function_def
                {
                    enum class type_t { pure, overridable, override };
                    std::string return_type;
                    std::string name;
                    std::vector<std::pair<std::string, std::string>> params; // type, name
                    type_t type{type_t::pure};
                    std::string content;

                    bool is_virtual() const { return type != type_t::pure; }
                    bool is_override() const { return type == type_t::override; }

                    std::string forward_to(const class_def& other) const
                    {
                        std::string s = "reinterpret_cast(this, " + other.name.fully_qualified_name() + ");";
                        if (return_type != "void" && return_type != "::void")
                        {
                            s += " return ";
                        }
                        s += namespace_t::fully_qualified_name(name) + "(this";
                        for (size_t i = 0; i < params.size(); ++i)
                        {
                            s += ", i" + std::to_string(i);
                        }
                        s += ");";
                        return s;
                    }

                    std::string declaration(const std::string& class_name,
                                            const std::vector<std::string>& param_names,
                                            const std::unordered_set<std::string>& ignored_types = {}) const
                    {
                        auto get_type = [&](const std::string& t) -> std::string {
                            if (t.empty()) return t;
                            bool has_ref = (t.back() == '&');
                            std::string base = has_ref ? t.substr(0, t.length() - 1) : t;
                            if (ignored_types.find(base) != ignored_types.end()) return t;
                            if (base.rfind(current_namespace_t::namespace_separator, 0) == 0) return t;
                            return current_namespace_t::of(base) + (has_ref ? "&" : "");
                        };

                        std::string d = get_type(return_type) + " " +
                                        namespace_t::fully_qualified_name(name) + "(" +
                                        namespace_t::fully_qualified_name(class_name) + "& this";
                        for (size_t i = 0; i < params.size(); ++i)
                        {
                            std::string p_name = (i < param_names.size()) ? param_names[i] : ("i" + std::to_string(i));
                            d += ", " + get_type(params[i].first) + " " + p_name;
                        }
                        d += ")";
                        return d;
                    }
                };

                name_with_namespace name;
                std::vector<parameter> vars_;
                std::vector<function_def> funcs_;
                std::vector<std::pair<std::string, function_def>> temps_;

                explicit class_def(std::string n)
                    : name(name_with_namespace::of(n)) {}

                class_def() = default;

                void with_var(const parameter& p)
                {
                    vars_.push_back(p);
                }

                bool check() const
                {
                    std::unordered_set<std::string> var_names;
                    for (const auto& v : vars_)
                    {
                        if (!var_names.insert(v.name).second)
                        {
                            raise_error("Duplicate variable in ", name.fully_qualified_name(), ": ", v.name);
                            return false;
                        }
                    }
                    std::unordered_set<std::string> func_sigs;
                    for (const auto& f : funcs_)
                    {
                        std::string sig = f.name;
                        for (const auto& p : f.params)
                        {
                            sig += ":" + p.first;
                        }
                        if (!func_sigs.insert(sig).second)
                        {
                            raise_error("Duplicate function in ", name.fully_qualified_name(), ": ", f.name);
                            return false;
                        }
                    }
                    return true;
                }

                void inherit_from(const class_def& other);
            };

            class scope::class_t
            {
            private:
                std::unordered_map<name_with_namespace, class_def> m_;

            public:
                bool define(const std::string& name, class_def cd)
                {
                    assert(!name.empty());
                    auto nwn = name_with_namespace::of(name);
                    if (!cd.check())
                    {
                        return false;
                    }
                    if (m_.find(nwn) != m_.end())
                    {
                        raise_error("Class ", name, " has been defined already.");
                        return false;
                    }
                    m_[nwn] = std::move(cd);
                    return true;
                }

                bool resolve(const std::string& name, class_def& o) const
                {
                    auto nwn = name_with_namespace::of(name);
                    auto it = m_.find(nwn);
                    if (it != m_.end())
                    {
                        o = it->second;
                        return true;
                    }
                    if (name.rfind(current_namespace_t::namespace_separator, 0) != 0)
                    {
                        auto gnwn = name_with_namespace::of_global_namespace(nwn.name);
                        it = m_.find(gnwn);
                        if (it != m_.end())
                        {
                            o = it->second;
                            return true;
                        }
                    }
                    return false;
                }

                bool is_defined(const std::string& name) const
                {
                    if (!with_namespace()) return false;
                    class_def dummy;
                    return resolve(name, dummy);
                }
            };

            class template_template
            {
            public:
                struct extended_type_name_t
                {
                    std::string name;
                    std::vector<std::string> types;

                    explicit extended_type_name_t(std::string n) : name(std::move(n)) {}

                    void apply(const std::vector<std::string>& ts)
                    {
                        types.clear();
                        for (const auto& t : ts)
                        {
                            std::string cleaned;
                            for (char c : t)
                            {
                                if (std::isalnum(static_cast<unsigned char>(c)) || c == '_')
                                {
                                    cleaned += c;
                                }
                                else
                                {
                                    cleaned += '_';
                                }
                            }
                            types.push_back(cleaned);
                        }
                    }

                    std::string str() const
                    {
                        std::string res = name;
                        for (const auto& t : types)
                        {
                            res += "__" + t;
                        }
                        return res;
                    }
                };

            private:
                std::string template_str_;
                extended_type_name_t ext_name_;
                std::vector<std::string> type_params_;

            public:
                template_template(const std::shared_ptr<automata::typed_node>& body,
                                  const std::shared_ptr<automata::typed_node>& name_node,
                                  const std::vector<std::string>& types)
                    : ext_name_(name_node->input_without_ignored()), type_params_(types)
                {
                    assert(body != nullptr);
                    assert(name_node != nullptr);
                    assert(!types.empty());

                    std::stringstream ss;
                    body->dfs(
                        [&ss, name_node, &types, this](const std::shared_ptr<automata::typed_node>& node,
                                                      const std::function<void()>& stop_nav) {
                            if (node == name_node)
                            {
                                ss << "@@@NAME@@@ ";
                                stop_nav();
                                return;
                            }
                            if (node->type_name == "raw-type-name")
                            {
                                std::string input_str = node->input_without_ignored();
                                for (size_t i = 0; i < types.size(); ++i)
                                {
                                    if (input_str == types[i])
                                    {
                                        ss << "@@@PARAM_" << i << "@@@ ";
                                        stop_nav();
                                        return;
                                    }
                                }
                            }
                        },
                        [&ss](const std::shared_ptr<automata::typed_node>& leaf) {
                            ss << leaf->word().str() << " ";
                        });
                    template_str_ = ss.str();
                }

                std::string extended_type_name(const std::vector<std::string>& types)
                {
                    ext_name_.apply(types);
                    return ext_name_.str();
                }

                bool apply(const std::vector<std::string>& types, std::string& impl)
                {
                    assert(!types.empty());
                    assert(types.size() == type_params_.size());
                    ext_name_.apply(types);
                    std::string res = template_str_;
                    std::string name_marker = "@@@NAME@@@";
                    size_t pos = 0;
                    std::string ext_name_str = ext_name_.str();
                    while ((pos = res.find(name_marker, pos)) != std::string::npos)
                    {
                        res.replace(pos, name_marker.length(), ext_name_str);
                        pos += ext_name_str.length();
                    }
                    for (size_t i = 0; i < types.size(); ++i)
                    {
                        std::string param_marker = "@@@PARAM_" + std::to_string(i) + "@@@";
                        pos = 0;
                        while ((pos = res.find(param_marker, pos)) != std::string::npos)
                        {
                            res.replace(pos, param_marker.length(), types[i]);
                            pos += types[i].length();
                        }
                    }
                    while (!res.empty() && res.back() == ' ')
                    {
                        res.pop_back();
                    }
                    impl = res;
                    return true;
                }
            };

            class scope::template_t
            {
            public:
                class definition
                {
                public:
                    std::shared_ptr<template_template> tmpl;
                    std::unordered_set<std::string> injected_types;

                    definition() = default;
                    explicit definition(std::shared_ptr<template_template> t)
                        : tmpl(std::move(t)) {}

                    bool apply(const std::vector<std::string>& types, std::string& o)
                    {
                        std::string key;
                        for (const auto& t : types) { key += t + ","; }
                        if (injected_types.insert(key).second)
                        {
                            return tmpl->apply(types, o);
                        }
                        o.clear();
                        return true;
                    }

                    std::string extended_type_name(const std::vector<std::string>& types)
                    {
                        return tmpl->extended_type_name(types);
                    }
                };

            private:
                std::unordered_map<name_with_namespace, definition> m_;

            public:
                bool define(const std::string& name, std::shared_ptr<template_template> t)
                {
                    assert(!name.empty());
                    assert(t != nullptr);
                    auto nwn = name_with_namespace::of(name);
                    if (m_.find(nwn) != m_.end())
                    {
                        raise_error("Template [", nwn.fully_qualified_name(), "] has been defined already.");
                        return false;
                    }
                    m_.emplace(nwn, definition(std::move(t)));
                    return true;
                }

                int resolve(const std::string& input_name,
                            const std::vector<std::string>& types,
                            std::string& extended_type_name);

                static std::string name_of(const std::string& name, uint32_t type_count)
                {
                    return name + "__" + std::to_string(type_count);
                }

                static uint32_t type_param_count(const std::shared_ptr<automata::typed_node>& n)
                {
                    assert(n != nullptr && n->child_count() == 2);
                    auto head = n->child(0);
                    assert(head != nullptr && head->child_count() == 4);
                    assert(!head->child(2)->leaf());
                    return head->child(2)->child_count();
                }

                static bool of(const std::shared_ptr<automata::typed_node>& n,
                               std::string& name,
                               std::shared_ptr<template_template>& t);

                static bool resolve(const std::shared_ptr<automata::typed_node>& n,
                                    std::string& extended_type_name);
            };

            inline scope::scope(scope* parent)
                : scope_base<scope>(parent)
            {
                if (is_root())
                {
                    defines().define("B3STYLE");
                    c_ = std::make_shared<class_t>();
                    tt_ = std::make_shared<template_t>();
                }
            }

            inline void class_def::inherit_from(const class_def& other)
            {
                vars_.insert(vars_.end(), other.vars_.begin(), other.vars_.end());

                for (const auto& f : other.funcs_)
                {
                    if (f.name == construct || f.name == destruct) continue;
                    if (!f.is_virtual())
                    {
                        function_def forwarded = f;
                        forwarded.content = forwarded.declaration(name.fully_qualified_name(), {}) + "{" + f.forward_to(other) + "}";
                        scope::current()->call_hierarchy().to(namespace_t::fully_qualified_name(f.name));
                        funcs_.push_back(forwarded);
                    }
                }
                for (const auto& f : other.funcs_)
                {
                    if (f.is_virtual())
                    {
                        bool overridden = false;
                        for (const auto& my_f : funcs_)
                        {
                            if (my_f.is_override() && my_f.name == f.name && my_f.params.size() == f.params.size())
                            {
                                bool types_match = true;
                                for (size_t p = 0; p < f.params.size(); ++p)
                                {
                                    if (my_f.params[p].first != f.params[p].first)
                                    {
                                        types_match = false;
                                        break;
                                    }
                                }
                                if (types_match)
                                {
                                    overridden = true;
                                    break;
                                }
                            }
                        }
                        if (!overridden)
                        {
                            function_def forwarded = f;
                            forwarded.content = forwarded.declaration(name.fully_qualified_name(), {}) + "{" + f.forward_to(other) + "}";
                            scope::current()->call_hierarchy().to(namespace_t::fully_qualified_name(f.name));
                            funcs_.push_back(forwarded);
                        }
                    }
                }
                for (const auto& t : other.temps_)
                {
                    const auto& f = t.second;
                    if (f.name == construct || f.name == destruct) continue;
                    if (!f.is_virtual())
                    {
                        function_def forwarded = f;
                        forwarded.content = forwarded.declaration(name.fully_qualified_name(), {}) + "{" + f.forward_to(other) + "}";
                        scope::current()->call_hierarchy().to(namespace_t::fully_qualified_name(f.name));
                        temps_.push_back({t.first, forwarded});
                    }
                }
            }

            inline int scope::template_t::resolve(const std::string& input_name,
                                                  const std::vector<std::string>& types,
                                                  std::string& extended_type_name)
            {
                assert(!types.empty());
                auto nwn = name_with_namespace::of(input_name);
                auto it = m_.find(nwn);
                if (it == m_.end())
                {
                    return 0; // unknown
                }
                std::string s;
                if (!it->second.apply(types, s))
                {
                    return -1; // false
                }
                if (!s.empty())
                {
                    auto pop_guard = nwn.ns.empty() ? nullptr : scope::current()->current_namespace().define(nwn.ns);
                    if (!build_code(s, *scope::current()->root_type_injector().current()))
                    {
                        return -1;
                    }
                }
                extended_type_name = namespace_t::fully_qualified_name(nwn.ns, it->second.extended_type_name(types));
                return 1; // true
            }

            inline std::vector<std::string> scope::value_target_t::with_temp_target(
                const std::string& type,
                logic_writer& o)
            {
                auto define_primitive_type_temp_target = [&](const std::string& t_raw, const std::string& n) {
                    std::string t = normalized_type::parameter_type_of(t_raw).full_type();
                    assert(!scope::current()->structs_is_type_defined(t));
                    assert(!scope::current()->variables_defined(n));
                    assert(scope::current()->variables().define(t, n));
                    assert(builders::of_define(n, t).to(o));
                };

                struct_def params;
                std::string temp_var_name = scope::current()->temp_logic_name().variable();
                if (scope::current()->structs_resolve(type, temp_var_name, params))
                {
                    for (const auto& p : params.primitives())
                    {
                        define_primitive_type_temp_target(p.non_ref_type(), p.name);
                    }
                    return with_value(type, params.primitives());
                }

                define_primitive_type_temp_target(type, temp_var_name);
                with_value(type, std::vector<std::string>{temp_var_name});
                return std::vector<std::string>{temp_var_name};
            }
        }
    }
}
