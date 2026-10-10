#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../code_gen/code_gen.hpp"
#include "compiler_scope_interface.hpp"
#include "features.hpp"
#include "accessor.hpp"
#include "normalized.hpp"
#include "includes.hpp"
#include "define.hpp"
#include "type_alias.hpp"
#include "struct.hpp"
#include "variable.hpp"
#include "function.hpp"
#include "value_target.hpp"
#include "params.hpp"
#include "current_function.hpp"
#include "delegate.hpp"
#include "temp_logic_name.hpp"
#include "call_hierarchy.hpp"
#include "namespace.hpp"
#include "name_with_namespace.hpp"
#include "class_def.hpp"
#include "template.hpp"
#include "root_type_injector.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename T>
        class scope_base
        {
        protected:
            inline static thread_local T* in_thread{nullptr};
            T* parent{nullptr};
            T* child{nullptr};
            std::vector<std::function<void()>> ds;
            bool end_scope_error_{false};

            explicit scope_base(T* parent_ptr)
                : parent(parent_ptr)
            {
                if (parent == nullptr)
                {
                    assert(in_thread == nullptr);
                }
                else
                {
                    assert(in_thread == parent);
                    assert(parent->child == nullptr);
                    parent->child = static_cast<T*>(this);
                }
                in_thread = static_cast<T*>(this);
            }

        public:
            void mark_end_scope_error()
            {
                end_scope_error_ = true;
            }

            bool has_end_scope_error() const
            {
                return end_scope_error_;
            }

            void run_end_scope()
            {
                while (!ds.empty())
                {
                    auto actions = std::move(ds);
                    ds.clear();
                    for (auto it = actions.rbegin(); it != actions.rend(); ++it)
                    {
                        if (*it) (*it)();
                    }
                }
            }

            virtual ~scope_base()
            {
                assert(in_thread == static_cast<T*>(this));
                run_end_scope();
                if (parent != nullptr)
                {
                    assert(parent->child == static_cast<T*>(this));
                    parent->child = nullptr;
                }
                in_thread = parent;
            }

            scope_base(const scope_base&) = delete;
            scope_base& operator=(const scope_base&) = delete;
            scope_base(scope_base&&) = delete;
            scope_base& operator=(scope_base&&) = delete;

            void when_end_scope(std::function<void()> a)
            {
                assert(static_cast<bool>(a));
                ds.push_back(std::move(a));
            }

            void disable_end_scope()
            {
                ds.clear();
            }

            bool is_root() const
            {
                return parent == nullptr;
            }

            static T* current()
            {
                assert(in_thread != nullptr);
                return in_thread;
            }

            static T* current_or_null()
            {
                return in_thread;
            }

            T* get_parent() const
            {
                return parent;
            }

            T* get_root()
            {
                T* s = static_cast<T*>(this);
                while (!s->is_root())
                {
                    s = s->parent;
                }
                return s;
            }

            const T* get_root() const
            {
                const T* s = static_cast<const T*>(this);
                while (!s->is_root())
                {
                    s = s->parent;
                }
                return s;
            }

            template <typename Getter>
            auto from_root(Getter&& getter) -> decltype(getter(static_cast<T*>(nullptr)))
            {
                return getter(get_root());
            }

            template <typename Getter>
            auto from_root(Getter&& getter) const -> decltype(getter(static_cast<const T*>(nullptr)))
            {
                return getter(get_root());
            }
        };

        template <typename Derived, typename Writer>
        class compiler_scope : public scope_base<Derived>, public compiler_scope_interface
        {
        public:
            using writer_t = Writer;
            using type_name = osi::compiler::type_name;
            using variable_name = osi::compiler::variable_name;
            using function_name = osi::compiler::function_name;
            using template_name = osi::compiler::template_name;
            using normalized_type = osi::compiler::normalized_type;
            using fully_qualified_variable_name = osi::compiler::fully_qualified_variable_name;
            using fully_qualified_function_name = osi::compiler::fully_qualified_function_name;
            using includes_t = osi::compiler::includes_t;
            using define_t = osi::compiler::define_t;
            using type_alias_t = osi::compiler::type_alias_t;
            using struct_t = osi::compiler::struct_t;
            using variable_t = osi::compiler::variable_t;
            using function_t = osi::compiler::function_t;
            using value_target_t = osi::compiler::value_target_t;
            using params_t = osi::compiler::params_t;
            using current_function_t = osi::compiler::current_function_t;
            using delegate_t = osi::compiler::delegate_t;
            using temp_logic_name_t = osi::compiler::temp_logic_name_t;
            using call_hierarchy_t = osi::compiler::call_hierarchy_t;
            using current_namespace_t = osi::compiler::current_namespace_t;
            using class_def = osi::compiler::class_def;
            using class_t = osi::compiler::class_t;
            using template_template = osi::compiler::template_template;
            using template_t = osi::compiler::template_t;
            using root_type_injector_t = osi::compiler::root_type_injector_t<Writer>;
            using accessor_t = osi::compiler::writer_accessor_t<Writer>;
            using features_t = osi::compiler::features_t;

            static bool code_build(const std::string& source, Writer& o)
            {
                return Derived::code_build(source, o);
            }

            static osi::compiler::code_gens<Writer>& code_gens()
            {
                return Derived::code_gens();
            }

            static typename osi::compiler::code_gens<Writer>::code_gen_proxy code_gen_of(
                const std::shared_ptr<automata::typed_node>& n)
            {
                return code_gens().of(n);
            }

        public:
            virtual accessor_t* get_accessor() = 0;
            virtual const accessor_t* get_accessor() const = 0;
            virtual const features_t* get_features() const
            {
                static features_t default_features;
                return &default_features;
            }

        protected:
            void init_root(template_t& tmpl)
            {
                tmpl.set_code_builder([](const std::string& s) {
                    return code_build(s, *Derived::current()->root_type_injector().current());
                });
            }

        public:
            explicit compiler_scope(Derived* parent = nullptr)
                : scope_base<Derived>(parent)
            {
                compiler_scope_interface::set_current(this);
            }

            ~compiler_scope() override
            {
                compiler_scope_interface::set_current(this->parent ? static_cast<compiler_scope_interface*>(this->parent) : nullptr);
            }

            accessor_t& myself() override
            {
                return *this->get_accessor();
            }

            const accessor_t& myself() const override
            {
                return *this->get_accessor();
            }

            const features_t& features() const override
            {
                return *this->get_root()->get_features();
            }

            bool is_root() const override
            {
                return scope_base<Derived>::is_root();
            }

            static Derived* current()
            {
                return scope_base<Derived>::current();
            }

            static Derived* current_or_null()
            {
                return scope_base<Derived>::current_or_null();
            }

            includes_t& includes() { return *this->get_root()->myself().includes(); }
            define_t& defines() { return *this->get_root()->myself().defines(); }
            function_t& functions() { return *this->get_root()->myself().functions(); }
            temp_logic_name_t& temp_logic_name() { return *this->get_root()->myself().temp_logic_name(); }
            call_hierarchy_t& call_hierarchy() { return *this->get_root()->myself().call_hierarchy(); }
            current_namespace_t& current_namespace() { return *this->get_root()->myself().current_namespace(); }
            root_type_injector_t& root_type_injector() { return *this->get_root()->myself().root_type_injector(); }
            class_t& classes() { return *this->get_root()->myself().classes(); }
            template_t& template_table() { return *this->get_root()->myself().template_table(); }

            type_alias_t& type_alias() { return *myself().type_alias(); }
            struct_t& structs() { return *myself().structs(); }
            variable_t& variables() { return *myself().variables(); }
            value_target_t& value_target() { return *myself().value_target(); }
            params_t& params() { return *myself().params(); }
            delegate_t& delegates() { return *myself().delegates(); }

            std::string resolve_type_node(const std::shared_ptr<automata::typed_node>& type) const override
            {
                assert(type != nullptr);
                if (type->type_name == "template-type-name" ||
                    (!type->leaf() && type->child_count() > 0 && type->child(0)->type_name == "template-type-name"))
                {
                    auto target_node = (type->type_name == "template-type-name") ? type : type->child(0);
                    std::string s;
                    if (code_gen_of(target_node).dump(s))
                    {
                        while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\n' || s.back() == '\r')) s.pop_back();
                        while (!s.empty() && (s.front() == ' ' || s.front() == '\t' || s.front() == '\n' || s.front() == '\r')) s.erase(s.begin());
                        if (!s.empty())
                        {
                            return s;
                        }
                    }
                }
                return "";
            }

            std::unique_ptr<Derived> start_scope()
            {
                return std::make_unique<Derived>(static_cast<Derived*>(this));
            }

            void without_end_scope()
            {
                this->disable_end_scope();
            }

            const current_function_t& current_function() const
            {
                const auto* cf = find_current_function();
                assert(cf != nullptr);
                return *cf;
            }

            void set_current_function(const std::string& name,
                                      const std::string& return_type,
                                      const std::vector<parameter>& params)
            {
                myself().current_function(current_function_t(name, return_type, params));
            }

            const current_function_t* find_current_function() const
            {
                const Derived* s = static_cast<const Derived*>(this);
                while (s != nullptr)
                {
                    const auto* cf = s->myself().current_function();
                    if (cf != nullptr && cf->has())
                    {
                        return cf;
                    }
                    s = s->get_parent();
                }
                return nullptr;
            }

            const std::string& current_function_return_type() const
            {
                const auto* cf = find_current_function();
                assert(cf != nullptr);
                return cf->return_type();
            }

            bool current_function_allow_return_value() const
            {
                const auto* cf = find_current_function();
                if (cf == nullptr) return false;
                return cf->allow_return_value();
            }

            std::string current_function_name_or(const std::string& def) const
            {
                const auto* cf = find_current_function();
                if (cf != nullptr && cf->has())
                {
                    return cf->name();
                }
                return def;
            }

            std::string current_function_signature_or(const std::string& default_sig) const
            {
                const auto* cf = find_current_function();
                if (cf != nullptr) return cf->signature();
                return default_sig;
            }

            std::string current_function_name() const override
            {
                return current_function_name_or("");
            }

            std::string type_alias(std::string alias) const override
            {
                assert(!parameter_type::is_ref_type(alias));
                if (!features().with_type_alias())
                {
                    return alias;
                }
                const Derived* s = static_cast<const Derived*>(this);
                while (s != nullptr)
                {
                    auto* ta = s->myself().type_alias();
                    if (ta != nullptr)
                    {
                        auto it = ta->map().find(alias);
                        if (it != ta->map().end())
                        {
                            alias = it->second;
                        }
                    }
                    s = s->get_parent();
                }
                return alias;
            }

            void type_alias_remove(const std::string& alias)
            {
                auto* ta = myself().type_alias();
                if (ta != nullptr)
                {
                    ta->remove(alias);
                }
            }

            bool variables_redefine(const std::string& type, const std::string& name)
            {
                if (variable_t::is_heap_name(name))
                {
                    raise_error("Redefine works for heap name without index, but got ", name, " (new type ", type, ")");
                    return false;
                }
                Derived* s = static_cast<Derived*>(this);
                while (s != nullptr)
                {
                    auto* v = s->myself().variables();
                    if (v != nullptr && v->redefine(type, name)) return true;
                    s = s->get_parent();
                }
                raise_error("Variable ", name, " (new type ", type, ") has not been defined yet.");
                return false;
            }

            bool variables_undefine(const std::string& name)
            {
                if (variable_t::is_heap_name(name))
                {
                    raise_error("Undefine works for heap name without index, but got ", name);
                    return false;
                }
                Derived* s = static_cast<Derived*>(this);
                while (s != nullptr)
                {
                    auto* v = s->myself().variables();
                    if (v != nullptr && v->undefine(name)) return true;
                    s = s->get_parent();
                }
                raise_error("Variable ", name, " has not been defined yet.");
                return false;
            }

            bool variables_resolve(std::string name,
                                   parameter& o,
                                   function_signature<parameter_type>* sig = nullptr) const
            {
                if (variable_t::is_heap_name(name))
                {
                    name = name.substr(0, name.find('['));
                }
                size_t pos = 0;
                while ((pos = name.find(".::", pos)) != std::string::npos)
                {
                    name.replace(pos, 3, ".");
                }
                const Derived* s = static_cast<const Derived*>(this);
                while (s != nullptr)
                {
                    auto* v = s->myself().variables();
                    if (v != nullptr && v->resolve(name, o))
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

            bool variable_defined(const std::string& name) const
            {
                return variables_defined(name);
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
                auto* v = myself().variables();
                assert(v != nullptr);
                for (const auto& p : vs)
                {
                    if (!v->define(p.unrefed_type(), p.name)) return false;
                }
                return true;
            }

            bool structs_resolve(const std::string& type, const std::string& name, struct_def& o) const override
            {
                const Derived* s = static_cast<const Derived*>(this);
                while (s != nullptr)
                {
                    auto* st = s->myself().structs();
                    if (st != nullptr && st->resolve(type, name, o)) return true;
                    s = s->get_parent();
                }
                return false;
            }

            bool structs_is_type_defined(const std::string& type) const override
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
                const Derived* s = static_cast<const Derived*>(this);
                while (s != nullptr)
                {
                    auto* de = s->myself().delegates();
                    if (de != nullptr && de->retrieve(name, o)) return true;
                    s = s->get_parent();
                }
                return false;
            }
        };
    }
}
