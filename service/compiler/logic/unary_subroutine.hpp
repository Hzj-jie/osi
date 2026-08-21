#pragma once
#include <string>
#include "data_ref_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class unary_subroutine : public data_ref_operator
            {
            protected:
                explicit unary_subroutine(std::string parameter)
                    : data_ref_operator({std::move(parameter)}) {}

                virtual bool parameter_restrict(const variable& parameter) const = 0;

                bool variable_restrict(uint32_t i, const variable& v) const override
                {
                    if (i == 0) return parameter_restrict(v);
                    assert(false);
                    return false;
                }
            };
        }
    }
}
