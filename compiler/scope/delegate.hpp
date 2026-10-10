#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "namespace.hpp"
#include "../function_signature.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"

namespace osi
{
    namespace compiler
    {
        class delegate_t
        {
        private:
            std::unordered_map<std::string, function_signature<parameter_type>> s_;

        public:
            bool define(const std::string& name,
                        const std::string& return_type,
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
                if (name.rfind(current_namespace_t::namespace_separator, 0) == 0)
                {
                    it = s_.find(name.substr(current_namespace_t::namespace_separator.size()));
                    if (it != s_.end())
                    {
                        o = it->second;
                        return true;
                    }
                }
                return false;
            }
        };
    }
}
