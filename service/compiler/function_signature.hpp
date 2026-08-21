#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "../../app_info/assert.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        class parameter_type
        {
        protected:
            std::string _type;

        public:
            bool ref{false};

            static bool is_ref_type(const std::string& type)
            {
                return !type.empty() && type.back() == '&' && type.size() > 1;
            }

            static std::string remove_ref(const std::string& type)
            {
                if (is_ref_type(type))
                {
                    size_t len = type.size() - 1;
                    while (len > 0 && (type[len - 1] == ' ' || type[len - 1] == '\t')) len--;
                    return type.substr(0, len);
                }
                return type;
            }

            parameter_type() = default;

            explicit parameter_type(const std::string& type)
            {
                assert(!type.empty());
                ref = is_ref_type(type);
                if (ref)
                {
                    _type = remove_ref(type);
                }
                else
                {
                    _type = type;
                }
            }

            parameter_type(std::string type, bool is_ref)
                : _type(std::move(type)), ref(is_ref)
            {
                assert(!_type.empty());
                assert(!is_ref_type(_type));
            }

            static parameter_type of(const std::string& type)
            {
                return parameter_type(type);
            }

            std::string full_type() const
            {
                if (ref) return _type + "&";
                return _type;
            }

            const std::string& non_ref_type() const
            {
                assert(!ref);
                return _type;
            }

            const std::string& unrefed_type() const
            {
                return _type;
            }

            std::string ToString() const
            {
                return full_type();
            }

            bool operator==(const parameter_type& other) const
            {
                return ref == other.ref && _type == other._type;
            }

            bool operator!=(const parameter_type& other) const
            {
                return !(*this == other);
            }
        };

        class parameter : public parameter_type
        {
        public:
            std::string name;

            parameter() = default;

            parameter(const std::string& type, std::string name)
                : parameter_type(type), name(std::move(name))
            {
                assert(!this->name.empty());
            }

            parameter(const std::string& type, bool is_ref, std::string name)
                : parameter_type(type, is_ref), name(std::move(name))
            {
                assert(!this->name.empty());
            }

            static parameter non_ref(const std::string& type, const std::string& name)
            {
                parameter p(type, name);
                assert(!p.ref);
                return p;
            }

            std::string ToString() const
            {
                return name + ": " + parameter_type::ToString();
            }

            bool operator==(const parameter& other) const
            {
                return parameter_type::operator==(other) && name == other.name;
            }

            bool operator!=(const parameter& other) const
            {
                return !(*this == other);
            }
        };

        template <typename ParamT = parameter_type>
        class function_signature
        {
        public:
            std::string name;
            std::string return_type;
            std::vector<ParamT> parameters;

            function_signature() = default;

            function_signature(std::string name, std::string return_type, std::vector<ParamT> parameters = {})
                : name(std::move(name)), return_type(std::move(return_type)), parameters(std::move(parameters))
            {
                assert(!this->name.empty());
                assert(!this->return_type.empty());
            }

            std::string ToString() const
            {
                std::string res = return_type + " " + name + "(";
                for (size_t i = 0; i < parameters.size(); ++i)
                {
                    if (i > 0) res += ", ";
                    res += parameters[i].ToString();
                }
                res += ")";
                return res;
            }
        };
    }
}
