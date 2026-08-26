#pragma once
#include <string>
#include "binary_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class binary_math_operator : public binary_operator
            {
            protected:
                binary_math_operator(std::string result, std::string left, std::string right)
                    : binary_operator(std::move(result), std::move(left), std::move(right)) {}

                bool parameter_restrict(const variable&) const override { return true; }
                bool result_restrict(const variable&) const override { return true; }
            };
        }
    }
}
