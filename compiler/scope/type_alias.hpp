#pragma once
#include <string>
#include <unordered_map>
#include "namespace.hpp"
#include "normalized.hpp"
#include "../function_signature.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"

namespace osi
{
    namespace compiler
    {
        class type_alias_t
        {
        private:
            std::unordered_map<std::string, std::string> m_;

        public:
            bool define(std::string alias, const std::string& canonical_type)
            {
                assert(!alias.empty());
                alias = current_namespace_t::of(alias);
                assert(!alias.empty());
                assert(!canonical_type.empty());
                parameter_type canonical = normalized_type::parameter_type_of(canonical_type);
                if (parameter_type::is_ref_type(alias))
                {
                    raise_error("Reference type ", alias, " is not allowed to be aliased.");
                    return false;
                }
                if (canonical.ref)
                {
                    raise_error("Reference type ", alias, " or canonical ", canonical.ToString(),
                                " is not allowed to be used as a canonical type.");
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

            void remove(const std::string& alias)
            {
                m_.erase(alias);
            }

            const std::unordered_map<std::string, std::string>& map() const
            {
                return m_;
            }
        };
    }
}
