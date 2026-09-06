#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../function_signature.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../../logic/scope.hpp"
#include "../scope.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class logic_name
            {
            public:
                static std::string of_function(const std::string& raw_name,
                                               const std::vector<parameter>& params)
                {
                    std::vector<std::string> types;
                    types.reserve(params.size());
                    for (const auto& p : params)
                    {
                        types.push_back(p.unrefed_type());
                    }
                    return build(raw_name, types);
                }

                static std::string of_function(const std::string& raw_name,
                                               const std::vector<parameter_type>& params)
                {
                    std::vector<std::string> types;
                    types.reserve(params.size());
                    for (const auto& p : params)
                    {
                        types.push_back(p.unrefed_type());
                    }
                    return build(raw_name, types);
                }

                static bool of_callee(const std::string& raw_name,
                                      std::string return_type,
                                      const std::vector<parameter>& parameters,
                                      const std::function<bool(logic_writer&)>& paragraph,
                                      logic_writer& o)
                {
                    std::string name = of_function(raw_name, parameters);
                    return_type = scope::normalized_type::of(return_type);
                    if (!scope::current()->functions().define(return_type, name))
                    {
                        return false;
                    }
                    scope::current()->set_current_function(name, return_type, parameters);
                    if (!scope::current()->variables_define(parameters))
                    {
                        return false;
                    }
                    std::string logic_return_type =
                        scope::current()->structs_is_type_defined(return_type)
                            ? logic::scope::type_t::variable_type
                            : return_type;
                    return builders::of_callee(name, logic_return_type, parameters, paragraph).to(o);
                }

                static bool of_function_call(const std::string& raw_name,
                                             const std::vector<std::string>& parameters,
                                             std::string& o)
                {
                    std::vector<std::string> types;
                    types.reserve(parameters.size());
                    for (const auto& p_name : parameters)
                    {
                        std::string t;
                        if (!scope::current()->variables_type_of(p_name, t))
                        {
                            return false;
                        }
                        types.push_back(t);
                    }
                    o = build(raw_name, types);
                    return true;
                }

            private:
                static std::string build(const std::string& raw_name, const std::vector<std::string>& types)
                {
                    std::string s = raw_name;
                    for (const auto& t : types)
                    {
                        assert(!parameter_type::is_ref_type(t));
                        s += ":" + scope::normalized_type::of(t);
                    }
                    return s;
                }
            };
        }
    }
}
