#pragma once
#include <string>
#include "unary_operator.hpp"
#include "unary_subroutine.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class _append : public unary_operator
            {
            public:
                _append(std::string result, std::string parameter)
                    : unary_operator(std::move(result), std::move(parameter)) {}

                std::string instruction_name() const override { return "app"; }
                bool parameter_restrict(const variable&) const override { return true; }
                bool result_restrict(const variable& result) const override { return result.is_variable_size(); }
            };

            class _not : public unary_operator
            {
            public:
                _not(std::string result, std::string parameter)
                    : unary_operator(std::move(result), std::move(parameter)) {}

                std::string instruction_name() const override { return "not"; }
                bool parameter_restrict(const variable&) const override { return true; }
                bool result_restrict(const variable& result) const override { return result.is_assignable_from_bool(); }
            };

            class _sizeof : public unary_operator
            {
            public:
                _sizeof(std::string result, std::string parameter)
                    : unary_operator(std::move(result), std::move(parameter)) {}

                std::string instruction_name() const override { return "sizeof"; }
                bool parameter_restrict(const variable&) const override { return true; }
                bool result_restrict(const variable& result) const override { return result.is_assignable_from_uint32(); }
            };

            class _empty : public unary_operator
            {
            public:
                _empty(std::string result, std::string parameter)
                    : unary_operator(std::move(result), std::move(parameter)) {}

                std::string instruction_name() const override { return "empty"; }
                bool parameter_restrict(const variable&) const override { return true; }
                bool result_restrict(const variable& result) const override { return result.is_assignable_from_bool(); }
            };

            class _clear : public unary_subroutine
            {
            public:
                explicit _clear(std::string parameter)
                    : unary_subroutine(std::move(parameter)) {}

                std::string instruction_name() const override { return "clr"; }
                bool parameter_restrict(const variable& parameter) const override { return parameter.is_variable_size(); }
            };
        }
    }
}
