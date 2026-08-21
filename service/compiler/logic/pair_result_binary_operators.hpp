#pragma once
#include <string>
#include "pair_result_binary_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class _divide : public pair_result_binary_operator
            {
            public:
                _divide(std::string result1, std::string result2, std::string left, std::string right)
                    : pair_result_binary_operator(std::move(result1), std::move(result2), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "div"; }
            };

            class _extract : public pair_result_binary_operator
            {
            public:
                _extract(std::string result1, std::string result2, std::string left, std::string right)
                    : pair_result_binary_operator(std::move(result1), std::move(result2), std::move(left), std::move(right)) {}
                std::string instruction_name() const override { return "ext"; }
            };
        }
    }
}
