#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "struct_def.hpp"
#include "namespace.hpp"
#include "normalized.hpp"
#include "compiler_scope_interface.hpp"
#include "accessor.hpp"
#include "type_alias.hpp"
#include "../logic/builders.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"

namespace osi
{
    namespace compiler
    {
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

            bool resolve_type(const std::string& type,
                              const std::string& name,
                              struct_def& d)
            {
                std::string full_type = normalized_type::parameter_type_of(type).full_type();
                auto it = s_.find(full_type);
                if (it == s_.end() && type.rfind(current_namespace_t::namespace_separator, 0) != 0)
                {
                    std::string global_type = current_namespace_t::namespace_separator + type;
                    it = s_.find(global_type);
                }
                if (it != s_.end())
                {
                    d.with_nested(it->first, name);
                    d.append(it->second.append_prefix(name));
                    return true;
                }
                auto* cs = compiler_scope_interface::current_or_null();
                struct_def sub_def;
                if (cs != nullptr && cs->structs_resolve(type, "", sub_def))
                {
                    d.with_nested(full_type, name);
                    d.append(sub_def.append_prefix(name));
                    return true;
                }
                d.with_primitive(full_type, name);
                return true;
            }

            bool define(const std::string& name,
                        std::vector<parameter> members,
                        const std::function<void(const std::string&, uint32_t)>& define_type)
            {
                assert(!name.empty());
                std::string full_type = normalized_type::parameter_type_of(name).full_type();
                assert(!parameter_type::is_ref_type(full_type));

                parameter type_id = create_type_id(full_type);
                define_type(normalized_type::of(type_id.unrefed_type()), 1);
                members.push_back(type_id);

                struct_def d;
                for (const auto& m : members)
                {
                    assert(!m.ref);
                    if (!resolve_type(m.non_ref_type(), m.name, d))
                    {
                        return false;
                    }
                }

                if (s_.find(full_type) != s_.end())
                {
                    raise_error("Struct type ", full_type, " has been defined already.");
                    return false;
                }
                s_[full_type] = std::move(d);
                auto* cs = compiler_scope_interface::current_or_null();
                if (cs != nullptr && cs->myself().type_alias() != nullptr)
                {
                    cs->myself().type_alias()->remove(full_type);
                }
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

            bool defined(const std::string& type) const
            {
                struct_def dummy;
                return resolve(type, "", dummy);
            }
        };
    }
}
