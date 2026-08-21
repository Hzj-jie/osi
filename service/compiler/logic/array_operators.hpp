#pragma once
#include <string>
#include "unary_operator.hpp"
#include "binary_operator.hpp"
#include "ternary_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class _append_slice : public unary_operator
            {
            public:
                _append_slice(std::string result, std::string parameter)
                    : unary_operator(std::move(result), std::move(parameter)) {}

                std::string instruction_name() const override { return "sapp"; }
                bool parameter_restrict(const variable&) const override { return true; }
                bool result_restrict(const variable&) const override { return true; }
            };

            class _cut_slice : public binary_operator
            {
            public:
                _cut_slice(std::string result, std::string left, std::string right)
                    : binary_operator(std::move(result), std::move(left), std::move(right)) {}

                std::string instruction_name() const override { return "scut"; }
                bool result_restrict(const variable&) const override { return true; }
                bool left_restrict(const variable&) const override { return true; }
                bool right_restrict(const variable& right) const override { return right.is_assignable_to_uint32(); }
            };

            class _cut : public binary_operator
            {
            public:
                _cut(std::string result, std::string left, std::string right)
                    : binary_operator(std::move(result), std::move(left), std::move(right)) {}

                std::string instruction_name() const override { return "cut"; }
                bool result_restrict(const variable&) const override { return true; }
                bool left_restrict(const variable&) const override { return true; }
                bool right_restrict(const variable& right) const override { return right.is_assignable_to_uint32(); }
            };

            class _cut_len : public ternary_operator
            {
            public:
                _cut_len(std::string result, std::string p1, std::string p2, std::string p3)
                    : ternary_operator(std::move(result), std::move(p1), std::move(p2), std::move(p3)) {}

                std::string instruction_name() const override { return "cutl"; }
                bool result_restrict(const variable&) const override { return true; }
                bool parameter1_restrict(const variable&) const override { return true; }
                bool parameter2_restrict(const variable& p2) const override { return p2.is_assignable_to_uint32(); }
                bool parameter3_restrict(const variable& p3) const override { return p3.is_assignable_to_uint32(); }
            };
        }
    }
}
