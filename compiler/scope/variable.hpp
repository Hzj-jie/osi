#pragma once
#include <string>
#include <unordered_map>
#include "namespace.hpp"
#include "normalized.hpp"
#include "compiler_scope_interface.hpp"
#include "../logic/builders.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"

namespace osi
{
    namespace compiler
    {
        class variable_t
        {
        private:
            std::unordered_map<std::string, parameter> s_;

        public:
            static bool is_heap_name(const std::string& name)
            {
                assert(!name.empty());
                size_t idx = name.find('[');
                if (idx != std::string::npos)
                {
                    assert(!name.empty() && name.back() == ']');
                    return true;
                }
                assert(name.find(']') == std::string::npos);
                return false;
            }

            bool define(std::string type, std::string name)
            {
                assert(!type.empty());
                assert(!name.empty());
                type = normalized_type::parameter_type_of(type).full_type();
                assert(!type.empty());
                name = fully_qualified_variable_name::of(name);
                assert(!name.empty());
                if (s_.find(name) != s_.end())
                {
                    raise_error("Variable ", name, " (new type ", type, ") has been defined already as ", s_[name].full_type());
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

            bool redefine(std::string type, const std::string& name)
            {
                assert(!type.empty());
                assert(!name.empty());
                type = normalized_type::parameter_type_of(type).full_type();
                assert(!type.empty());
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

            bool defined_locally(const std::string& name) const
            {
                return s_.find(name) != s_.end() || s_.find(current_namespace_t::of(name)) != s_.end();
            }
        };
    }
}
