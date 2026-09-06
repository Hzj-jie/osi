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
#include "../code_gen/statements.hpp"
#include "b2style_forward.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            class scope;

            // Forward declaration of build_code for recursive code builds (templates, classes, etc.)
            bool build_code(const std::string& input, rewriter::typed_node_writer& o);

            class current_file_guard
            {
            private:
                static std::vector<std::string>& stack()
                {
                    static thread_local std::vector<std::string> s;
                    return s;
                }
            public:
                static std::string get()
                {
                    if (!stack().empty())
                    {
                        return stack().back();
                    }
                    return "unknown_file";
                }

                explicit current_file_guard(const std::string& file)
                {
                    stack().push_back(file);
                }

                ~current_file_guard()
                {
                    if (!stack().empty())
                    {
                        stack().pop_back();
                    }
                }
            };

            class current_namespace_t
            {
            private:
                std::vector<std::string> s_;

            public:
                inline static const std::string namespace_separator = "::";

                struct defer_pop
                {
                    current_namespace_t* self;
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

                static std::pair<std::string, std::string> of_namespace_and_name(const std::string& i)
                {
                    std::string f = of(i);
                    size_t index = f.rfind(namespace_separator);
                    if (index == std::string::npos)
                    {
                        return {"", f};
                    }
                    return {f.substr(0, index), f.substr(index + namespace_separator.length())};
                }

                const std::vector<std::string>& stack() const { return s_; }
                std::vector<std::string>& stack() { return s_; }
            };

            class namespace_t
            {
            public:
                static std::string fully_qualified_name(const std::string& n)
                {
                    assert(!n.empty());
                    if (n.rfind(current_namespace_t::namespace_separator, 0) == 0)
                    {
                        return n;
                    }
                    return current_namespace_t::namespace_separator + n;
                }

                static std::string fully_qualified_name(const std::string& n, const std::string& i)
                {
                    assert(!i.empty());
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
    struct hash<osi::compiler::b2style_compiler::name_with_namespace>
    {
        size_t operator()(const osi::compiler::b2style_compiler::name_with_namespace& n) const
        {
            return std::hash<std::string>()(n.ns) ^ (std::hash<std::string>()(n.name) << 1);
        }
    };
}

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {

            namespace _namespace
            {
                class bstyle_format
                {
                private:
                    inline static const std::string namespace_replacer = "__";

                public:
                    static std::string of(const std::string& i)
                    {
                        if (i.empty()) return i;
                        std::string base = current_namespace_t::of(i);
                        std::vector<std::string> parts;
                        std::stringstream ss(base);
                        std::string item;
                        while (std::getline(ss, item, '.'))
                        {
                            parts.push_back(item);
                        }
                        if (parts.empty()) parts.push_back(base);

                        std::string res;
                        for (size_t p = 0; p < parts.size(); ++p)
                        {
                            if (p > 0) res += '.';
                            const std::string& x = parts[p];
                            if (x.find(current_namespace_t::namespace_separator) == std::string::npos)
                            {
                                res += x;
                            }
                            else
                            {
                                std::string q = current_namespace_t::of(x);
                                size_t pos = 0;
                                while ((pos = q.find(current_namespace_t::namespace_separator, pos)) != std::string::npos)
                                {
                                    q.replace(pos, current_namespace_t::namespace_separator.length(), namespace_replacer);
                                    pos += namespace_replacer.length();
                                }
                                if (q.rfind(namespace_replacer, 0) == 0)
                                {
                                    q = q.substr(namespace_replacer.length());
                                }
                                res += q;
                            }
                        }
                        return res;
                    }

                    static std::string of(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        return of(n->input_without_ignored());
                    }

                    static std::string fully_qualified_name(const std::string& i)
                    {
                        return of(namespace_t::fully_qualified_name(i));
                    }

                    static std::string operator_function_name(const std::string& operator_name)
                    {
                        assert(!operator_name.empty());
                        std::string op = operator_name;
                        for (char& c : op)
                        {
                            if (c == '-') c = '_';
                        }
                        return of(namespace_t::fully_qualified_name("b2style", op));
                    }
                };
            }

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

                void to_bstyle_function(const std::string& name);

                void to(const std::string& name)
                {
                    to_bstyle_function(_namespace::bstyle_format::of(name));
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

                static std::function<bool(const std::shared_ptr<automata::typed_node>&)> from_value_clause();

                class calculator : public statement<rewriter::typed_node_writer>
                {
                public:
                    static void register_statement(statements<rewriter::typed_node_writer>& p)
                    {
                        static std::shared_ptr<calculator> instance = std::make_shared<calculator>();
                        p.register_statement(instance);
                    }
                    void export_to(rewriter::typed_node_writer&) override;
                };
            };

            class variable_t
            {
            private:
                std::unordered_map<std::string, std::string> s_;

            public:
                bool define(const std::string& type, std::string name);
                bool defined(const std::string& name) const;
                bool resolve(const std::string& name, std::string& type) const;
            };

            class variable_proxy
            {
            public:
                static std::function<bool(const std::shared_ptr<automata::typed_node>&)> define();
            };

            class root_type_injector_t
            {
            private:
                std::shared_ptr<rewriter::typed_node_writer> current_;

            public:
                void _new(rewriter::typed_node_writer& o)
                {
                    current_ = std::make_shared<rewriter::typed_node_writer>();
                    o.append(current_);
                }

                std::shared_ptr<rewriter::typed_node_writer> current()
                {
                    assert(current_ != nullptr);
                    return current_;
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
                rewriter::typed_node_writer w_;
                extended_type_name_t ext_name_;
                std::vector<std::shared_ptr<std::string>> type_refs_;

            public:
                template_template(const std::shared_ptr<automata::typed_node>& body,
                                  const std::shared_ptr<automata::typed_node>& name_node,
                                  const std::vector<std::string>& types)
                    : ext_name_(name_node->input_without_ignored())
                {
                    assert(body != nullptr);
                    assert(name_node != nullptr);
                    assert(!types.empty());

                    type_refs_.resize(types.size());
                    for (size_t i = 0; i < types.size(); ++i)
                    {
                        type_refs_[i] = std::make_shared<std::string>();
                    }

                    body->dfs(
                        [this, name_node, types](const std::shared_ptr<automata::typed_node>& node,
                                                 const std::function<void()>& stop_nav) {
                            if (node == name_node)
                            {
                                w_.append([this]() { return ext_name_.str(); });
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
                                        auto ref = type_refs_[i];
                                        w_.append([ref]() { return *ref; });
                                        stop_nav();
                                        return;
                                    }
                                }
                            }
                        },
                        [this](const std::shared_ptr<automata::typed_node>& leaf) {
                            w_.append(leaf->word().str());
                        });
                }

                std::string extended_type_name(const std::vector<std::string>& types)
                {
                    ext_name_.apply(types);
                    return ext_name_.str();
                }

                bool apply(const std::vector<std::string>& types, std::string& impl)
                {
                    assert(!types.empty());
                    assert(types.size() == type_refs_.size());
                    ext_name_.apply(types);
                    for (size_t i = 0; i < types.size(); ++i)
                    {
                        *type_refs_[i] = types[i];
                    }
                    impl = w_.str();
                    for (size_t i = 0; i < types.size(); ++i)
                    {
                        type_refs_[i]->clear();
                    }
                    return true;
                }
            };

            class template_t
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
            };

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
                            if (ignored_types.find(t) != ignored_types.end()) return t;
                            return namespace_t::fully_qualified_name(t);
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
                std::vector<std::pair<std::string, function_def>> temps_; // template_head, func

                explicit class_def(std::string n)
                    : name(name_with_namespace::of(n)) {}

                class_def() = default;

                void with_var(const parameter& p)
                {
                    vars_.push_back(p);
                }

                void inherit_from(const class_def& other);
            };

            class class_t
            {
            private:
                std::unordered_map<std::string, class_def> m_;

            public:
                bool define(const std::string& name, class_def cd)
                {
                    assert(!name.empty());
                    m_[name] = std::move(cd);
                    return true;
                }

                bool resolve(const std::string& name, class_def& o) const
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

            class current_function_proxy
            {
            private:
                scope* s_{nullptr};

            public:
                explicit current_function_proxy(scope* s) : s_(s) { assert(s_ != nullptr); }

                scope* define(const std::string& name);
                std::string name() const;
            };

            class scope : public scope_base<scope>
            {
            private:
                includes_t incs_;
                call_hierarchy_t fc_;
                current_namespace_t cn_;
                define_t d_;
                root_type_injector_t i_;
                template_t t_;
                variable_t v_;
                class_t c_;
                std::string cur_func_;

            public:
                explicit scope(scope* parent)
                    : scope_base<scope>(parent)
                {
                    if (parent != nullptr)
                    {
                        // Inherit or share references
                    }
                }

                scope() : scope(nullptr)
                {
                    d_.define("B2STYLE");
                }

                static scope* current()
                {
                    return scope_base<scope>::current();
                }

                includes_t& includes()
                {
                    return get_root()->incs_;
                }

                call_hierarchy_t& call_hierarchy()
                {
                    return get_root()->fc_;
                }

                current_namespace_t& current_namespace()
                {
                    return get_root()->cn_;
                }

                define_t& defines()
                {
                    return get_root()->d_;
                }

                root_type_injector_t& root_type_injector()
                {
                    return get_root()->i_;
                }

                template_t& template_table()
                {
                    return get_root()->t_;
                }

                class_t& classes()
                {
                    return get_root()->c_;
                }

                variable_t& variables()
                {
                    return v_;
                }

                bool has_current_function() const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (!s->cur_func_.empty()) return true;
                        s = s->get_parent();
                    }
                    return false;
                }

                std::string current_function_name() const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (!s->cur_func_.empty()) return s->cur_func_;
                        s = s->get_parent();
                    }
                    return "";
                }

                std::string current_function_name_or(const std::string& def) const
                {
                    std::string f = current_function_name();
                    return f.empty() ? def : f;
                }

                current_function_proxy current_function()
                {
                    return current_function_proxy(this);
                }

                void set_current_function_name(const std::string& name)
                {
                    cur_func_ = name;
                }

                bool variable_defined(const std::string& name) const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->v_.defined(name)) return true;
                        s = s->get_parent();
                    }
                    return false;
                }

                bool variable_resolve(const std::string& name, std::string& type) const
                {
                    const scope* s = this;
                    while (s != nullptr)
                    {
                        if (s->v_.resolve(name, type)) return true;
                        s = s->get_parent();
                    }
                    return false;
                }
            };

            // Implementations of inline/deferred methods
            inline std::shared_ptr<current_namespace_t::defer_pop> current_namespace_t::define(const std::string& name)
            {
                assert(!name.empty());
                std::string n = name;
                if (n.rfind(namespace_separator, 0) != 0)
                {
                    n = namespace_separator + n;
                }
                s_.push_back(n);
                auto d = std::make_shared<defer_pop>();
                d->self = this;
                return d;
            }

            inline std::string current_namespace_t::of(const std::string& i)
            {
                assert(!i.empty());
                if (i.rfind(namespace_separator, 0) == 0)
                {
                    return i;
                }
                scope* cur = scope::current();
                if (cur != nullptr)
                {
                    auto& st = cur->current_namespace().stack();
                    if (!st.empty())
                    {
                        return st.back() + namespace_separator + i;
                    }
                }
                return namespace_separator + i;
            }

            inline bool variable_t::define(const std::string& type, std::string name)
            {
                assert(!type.empty());
                assert(!name.empty());
                if (scope::current()->is_root())
                {
                    name = current_namespace_t::of(name);
                }
                if (s_.find(name) != s_.end())
                {
                    raise_error("Variable ", name, " has already been defined.");
                    return false;
                }
                s_[name] = type;
                return true;
            }

            inline bool variable_t::defined(const std::string& name) const
            {
                std::string dummy;
                return resolve(name, dummy);
            }

            inline bool variable_t::resolve(const std::string& name, std::string& type) const
            {
                assert(!name.empty());
                auto it = s_.find(name);
                if (it != s_.end())
                {
                    type = it->second;
                    return true;
                }
                std::string fn = current_namespace_t::of(name);
                if (fn != name)
                {
                    it = s_.find(fn);
                    if (it != s_.end())
                    {
                        type = it->second;
                        return true;
                    }
                }
                return false;
            }

            inline void call_hierarchy_t::to_bstyle_function(const std::string& name)
            {
                assert(!name.empty());
                std::string from = scope::current()->current_function_name_or(state_->main_name);
                if (name != from)
                {
                    state_->m_[from].push_back(name);
                }
            }

            inline std::function<bool(const std::shared_ptr<automata::typed_node>&)> call_hierarchy_t::from_value_clause()
            {
                return [](const std::shared_ptr<automata::typed_node>& n) -> bool {
                    assert(n != nullptr);
                    std::shared_ptr<automata::typed_node> o;
                    if (n->last_child()->only_descendant(o) && o->type_name == "raw-name")
                    {
                        scope::current()->call_hierarchy().to(o->word().str());
                    }
                    return true;
                };
            }

            inline void call_hierarchy_t::calculator::export_to(rewriter::typed_node_writer&)
            {
                scope::current()->call_hierarchy().calculate();
            }

            inline std::function<bool(const std::shared_ptr<automata::typed_node>&)> variable_proxy::define()
            {
                return [](const std::shared_ptr<automata::typed_node>& n) -> bool {
                    assert(n != nullptr);
                    assert(n->child_count() >= 2);
                    return scope::current()->variables().define(
                        n->child(0)->input_without_ignored(),
                        n->child(1)->input_without_ignored());
                };
            }

            inline scope* current_function_proxy::define(const std::string& name)
            {
                assert(!name.empty());
                s_->set_current_function_name(name);
                return s_;
            }

            inline std::string current_function_proxy::name() const
            {
                return s_->current_function_name();
            }

            inline int template_t::resolve(const std::string& input_name,
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

            inline void class_def::inherit_from(const class_def& other)
            {
                vars_.insert(vars_.end(), other.vars_.begin(), other.vars_.end());

                // Forward non-overrides (regular methods)
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
                // Forward virtual methods if not overridden
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
                // Forward templates
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
                for (const auto& t : other.temps_)
                {
                    const auto& f = t.second;
                    if (f.is_virtual())
                    {
                        bool overridden = false;
                        for (const auto& my_t : temps_)
                        {
                            if (my_t.second.is_override() && my_t.second.name == f.name && my_t.second.params.size() == f.params.size())
                            {
                                bool types_match = true;
                                for (size_t p = 0; p < f.params.size(); ++p)
                                {
                                    if (my_t.second.params[p].first != f.params[p].first)
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
                            temps_.push_back({t.first, forwarded});
                        }
                    }
                }
            }
        }
    }
}
