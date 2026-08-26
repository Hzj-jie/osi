#pragma once
#include <string>
#include "data_ref_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class unary_operator : public data_ref_operator
            {
            protected:
                unary_operator(std::string result, std::string parameter)
                    : data_ref_operator({std::move(result), std::move(parameter)}) {}

                virtual bool result_restrict(const variable& result) const = 0;
                virtual bool parameter_restrict(const variable& parameter) const = 0;

                bool variable_restrict(uint32_t i, const variable& v) const override
                {
                    if (i == 0) return result_restrict(v);
                    if (i == 1) return parameter_restrict(v);
                    assert(false);
                    return false;
                }
            };
        }
    }
}
