#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <utility>
#include "name_with_namespace.hpp"
#include "../logic/builders.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"

namespace osi
{
    namespace compiler
    {
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
                        return current_namespace_t::namespace_separator + base + (has_ref ? "&" : "");
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

            void inherit_from(const class_def& other)
            {
                vars_.insert(vars_.end(), other.vars_.begin(), other.vars_.end());

                auto* cur = compiler_scope_interface::current_or_null();
                auto* ch = cur ? cur->myself().call_hierarchy() : nullptr;

                for (const auto& f : other.funcs_)
                {
                    if (f.name == construct || f.name == destruct) continue;
                    if (!f.is_virtual())
                    {
                        function_def forwarded = f;
                        forwarded.content = forwarded.declaration(name.fully_qualified_name(), {}) + "{" + f.forward_to(other) + "}";
                        if (ch) ch->to(namespace_t::fully_qualified_name(f.name));
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
                            if (ch) ch->to(namespace_t::fully_qualified_name(f.name));
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
                        if (ch) ch->to(namespace_t::fully_qualified_name(f.name));
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
                            if (ch) ch->to(namespace_t::fully_qualified_name(f.name));
                            temps_.push_back({t.first, forwarded});
                        }
                    }
                }
            }
        };

        class class_t
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
                class_def dummy;
                return resolve(name, dummy);
            }
        };
    }
}
