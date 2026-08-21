#pragma once
#include <string>
#include "binary_math_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class _add : public binary_math_operator
            {
            public:
                _add(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "add"; }
            };

            class _subtract : public binary_math_operator
            {
            public:
                _subtract(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "sub"; }
            };

            class _multiply : public binary_math_operator
            {
            public:
                _multiply(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "mul"; }
            };

            class _power : public binary_math_operator
            {
            public:
                _power(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "pow"; }
            };

            class _and : public binary_math_operator
            {
            public:
                _and(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "and"; }
            };

            class _or : public binary_math_operator
            {
            public:
                _or(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "or"; }
            };

            class _float_add : public binary_math_operator
            {
            public:
                _float_add(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fadd"; }
            };

            class _float_subtract : public binary_math_operator
            {
            public:
                _float_subtract(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fsub"; }
            };

            class _float_multiply : public binary_math_operator
            {
            public:
                _float_multiply(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fmul"; }
            };

            class _float_power : public binary_math_operator
            {
            public:
                _float_power(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fpow"; }
            };

            class _float_divide : public binary_math_operator
            {
            public:
                _float_divide(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fdiv"; }
            };

            class _float_extract : public binary_math_operator
            {
            public:
                _float_extract(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "fext"; }
            };

            class _left_shift : public binary_math_operator
            {
            public:
                _left_shift(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "lfs"; }
            };

            class _right_shift : public binary_math_operator
            {
            public:
                _right_shift(std::string result, std::string left, std::string right)
                    : binary_math_operator(std::move(result), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "rfs"; }
            };
        }
    }
}
