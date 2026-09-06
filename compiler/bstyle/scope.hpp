#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <queue>
#include <functional>
#include "../scope/scope.hpp"
#include "../scope/struct_def.hpp"
#include "../read_scoped.hpp"
#include "../function_signature.hpp"
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

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class scope : public scope_base<scope>
            {
            public:
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
                        return parameter_type_of(type->input_without_ignored());
                    }

                    static std::string of(const std::string& type)
                    {
                        return current()->type_alias(type);
                    }

                    static std::string of(const std::shared_ptr<automata::typed_node>& type)
                    {
                        assert(type != nullptr);
                        return of(type->input_without_ignored());
                    }
                };

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
                        return n->input_without_ignored();
                    }
                };

                struct fully_qualified_variable_name
                {
                    static std::string of(const std::string& name)
                    {
                        return name;
                    }

                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return n->input_without_ignored();
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

                struct fully_qualified_function_name
                {
                    static std::string of(const std::string& name)
                    {
                        return name;
                    }

                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return n->input_without_ignored();
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
                    bool define(const std::string& alias, const std::string& canonical_type)
                    {
                        assert(!alias.empty());
                        assert(!canonical_type.empty());
                        auto canonical = normalized_type::parameter_type_of(canonical_type);
                        if (parameter_type::is_ref_type(alias))
                        {
                            raise_error("Reference type ", alias, " is not allowed to be aliased.");
                            return false;
                        }
                        if (canonical.ref)
                        {
                            raise_error("Reference type is not allowed to be used as canonical type.");
                            return false;
                        }
                        if (alias == canonical.non_ref_type())
                        {
                            raise_error("Cycle typedefs detected: ", alias);
                            return false;
                        }
                        m_[alias] = canonical.non_ref_type();
                        return true;
                    }

                    void remove(const std::string& alias)
                    {
                        m_.erase(alias);
                    }

                    const std::unordered_map<std::string, std::string>& map() const
                    {
                        return m_;
                    }
                };

                class variable_t
                {
                private:
                    std::unordered_map<std::string, std::string> s_;

                public:
                    bool define(const std::string& type, const std::string& name)
                    {
                        assert(!type.empty());
                        assert(!name.empty());
                        assert(!parameter_type::is_ref_type(type));
                        if (s_.find(name) != s_.end())
                        {
                            raise_error("Variable ", name, " has already been defined as ", s_[name]);
                            return false;
                        }
                        s_[name] = type;
                        return true;
                    }

                    bool redefine(const std::string& type, const std::string& name)
                    {
                        assert(!type.empty());
                        assert(!name.empty());
                        auto it = s_.find(name);
                        if (it != s_.end())
                        {
                            it->second = type;
                            return true;
                        }
                        return false;
                    }

                    bool undefine(const std::string& name)
                    {
                        return s_.erase(name) > 0;
                    }

                    bool resolve(const std::string& name, parameter& o) const
                    {
                        auto it = s_.find(name);
                        if (it != s_.end())
                        {
                            o = parameter(it->second, name);
                            return true;
                        }
                        return false;
                    }

                    bool defined_locally(const std::string& name) const
                    {
                        return s_.find(name) != s_.end();
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
                        return parameter::non_ref(name + "__struct__type__id__type", name + "__struct__type__id");
                    }

                    bool define(const std::string& name,
                                std::vector<parameter> members,
                                const std::function<void(const std::string&, uint32_t)>& define_type)
                    {
                        assert(!name.empty());
                        std::string full_type = normalized_type::parameter_type_of(name).full_type();
                        assert(!parameter_type::is_ref_type(full_type));

                        parameter type_id = create_type_id(name);
                        define_type(normalized_type::of(type_id.unrefed_type()), 1);
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

                    bool resolve(const std::string& type, const std::string& name, struct_def& o) const
                    {
                        std::string full_type = normalized_type::parameter_type_of(type).full_type();
                        auto it = s_.find(full_type);
                        if (it == s_.end())
                        {
                            return false;
                        }
                        if (!name.empty())
                        {
                            o = it->second.append_prefix(name);
                        }
                        else
                        {
                            o = it->second;
                        }
                        return true;
                    }
                };

                class function_t
                {
                private:
                    std::unordered_map<std::string, std::string> s_;

                public:
                    bool define(const std::string& return_type, const std::string& name)
                    {
                        assert(!return_type.empty());
                        assert(!name.empty());
                        if (s_.find(name) != s_.end())
                        {
                            raise_error("Function ", name, " has already been defined with return type ", s_[name]);
                            return false;
                        }
                        s_[name] = return_type;
                        return true;
                    }

                    bool is_defined(const std::string& name) const
                    {
                        return s_.find(name) != s_.end();
                    }

                    bool return_type_of(const std::string& name, std::string& type) const
                    {
                        auto it = s_.find(name);
                        if (it != s_.end())
                        {
                            type = it->second;
                            return true;
                        }
                        raise_error("Function ", name, " has not been defined.");
                        return false;
                    }
                };

                class params_t
                {
                private:
                    std::vector<parameter> v_;

                public:
                    void pack(const std::vector<parameter>& n)
                    {
                        v_.insert(v_.end(), n.begin(), n.end());
                    }

                    std::vector<parameter> unpack()
                    {
                        std::vector<parameter> res = std::move(v_);
                        v_.clear();
                        return res;
                    }
                };

                class current_function_t
                {
                private:
                    function_signature<parameter> sig_;
                    bool has_{false};

                public:
                    current_function_t() = default;

                    void define(const std::string& name,
                                const std::string& return_type,
                                const std::vector<parameter>& params)
                    {
                        sig_ = function_signature<parameter>(
                            name,
                            normalized_type::parameter_type_of(return_type).full_type(),
                            params);
                        has_ = true;
                    }

                    bool has() const { return has_; }

                    bool allow_return_value() const
                    {
                        return sig_.return_type != logic::scope::type_t::zero_type && sig_.return_type != "void";
                    }

                    const std::string& name() const { return sig_.name; }
                    const std::string& return_type() const { return sig_.return_type; }
                    std::string signature() const { return sig_.ToString(); }
                    bool return_struct() const;
                };

                class delegate_t
                {
                private:
                    std::unordered_map<std::string, function_signature<parameter_type>> s_;

                public:
                    bool define(const std::string& return_type,
                                const std::string& name,
                                const std::vector<parameter_type>& params)
                    {
                        assert(!name.empty());
                        if (s_.find(name) != s_.end())
                        {
                            raise_error("Delegate type ", name, " has already been defined.");
                            return false;
                        }
                        s_[name] = function_signature<parameter_type>(name, return_type, params);
                        return true;
                    }

                    bool retrieve(const std::string& name, function_signature<parameter_type>& o) const
                    {
                        auto it = s_.find(name);
                        if (it != s_.end())
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
                    uint32_t id_{0};

                public:
                    std::string variable()
                    {
                        id_++;
                        return "temp_value_@" + std::to_string(id_);
                    }
                };

                class value_target_t
                {
                public:
                    struct target
                    {
                        std::string type;
                        std::vector<std::string> names;
                    };

                private:
                    read_scoped<target> values_;
                    read_scoped<std::vector<std::string>> value_lists_;

                public:
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

                    void to(const std::string& name)
                    {
                        assert(!name.empty());
                        std::string from = current()->current_function_name_or(state_->main_name);
                        if (name != from)
                        {
                            state_->m_[from].push_back(name);
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

            public:
                explicit scope(scope* parent = nullptr)
                    : scope_base<scope>(parent)
                {
                    if (is_root())
                    {
                        defines().define("BSTYLE");
                    }
                }

                includes_t& includes() { return get_root()->incs_; }
                define_t& defines() { return get_root()->d_; }
                function_t& functions() { return get_root()->f_; }
                temp_logic_name_t& temp_logic_name() { return get_root()->t_; }
                call_hierarchy_t& call_hierarchy() { return get_root()->fc_; }

                type_alias_t& type_alias() { return ta_; }
                struct_t& structs() { return s_; }
                variable_t& variables() { return v_; }
                value_target_t& value_target() { return vt_; }
                params_t& params() { return ps_; }
                delegate_t& delegates() { return de_; }

                void set_current_function(const std::string& name,
                                          const std::string& return_type,
                                          const std::vector<parameter>& params)
                {
                    cf_.define(name, return_type, params);
                }

                const current_function_t* find_current_function() const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->cf_.has()) return &s->cf_;
                        s = s->get_parent();
                    }
                    return nullptr;
                }

                std::string current_function_name_or(const std::string& default_name) const
                {
                    const auto* cf = find_current_function();
                    if (cf != nullptr) return cf->name();
                    return default_name;
                }

                std::string current_function_signature_or(const std::string& default_sig) const
                {
                    const auto* cf = find_current_function();
                    if (cf != nullptr) return cf->signature();
                    return default_sig;
                }

                const current_function_t& current_function() const
                {
                    const auto* cf = find_current_function();
                    assert(cf != nullptr);
                    return *cf;
                }

                std::unique_ptr<scope> start_scope()
                {
                    return std::make_unique<scope>(this);
                }

                void without_end_scope()
                {
                    disable_end_scope();
                }

                bool current_function_allow_return_value() const
                {
                    const auto* cf = find_current_function();
                    assert(cf != nullptr);
                    return cf->allow_return_value();
                }

                const std::string& current_function_return_type() const
                {
                    const auto* cf = find_current_function();
                    assert(cf != nullptr);
                    return cf->return_type();
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
                    ta_.remove(alias);
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
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->s_.resolve(type, name, o)) return true;
                        s = s->get_parent();
                    }
                    return false;
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
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->de_.retrieve(name, o)) return true;
                        s = s->get_parent();
                    }
                    return false;
                }
            };

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

            inline bool scope::current_function_t::return_struct() const
            {
                return scope::current()->structs_is_type_defined(return_type());
            }
        }
    }
}
