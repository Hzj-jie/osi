#pragma once
#include <string>
#include <unordered_map>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"

namespace osi
{
    namespace compiler
    {
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
    }
}
