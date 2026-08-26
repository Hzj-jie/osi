#pragma once
#include <string>
#include "data_ref_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class binary_operator : public data_ref_operator
            {
            protected:
                binary_operator(std::string result, std::string left, std::string right)
                    : data_ref_operator({std::move(result), std::move(left), std::move(right)}) {}

                virtual bool result_restrict(const variable& result) const = 0;
                virtual bool left_restrict(const variable& left) const { return parameter_restrict(left); }
                virtual bool right_restrict(const variable& right) const { return parameter_restrict(right); }
                virtual bool parameter_restrict(const variable& param) const { return true; }

                bool variable_restrict(uint32_t i, const variable& v) const override
                {
                    if (i == 0) return result_restrict(v);
                    if (i == 1) return left_restrict(v);
                    if (i == 2) return right_restrict(v);
                    assert(false);
                    return false;
                }
            };
        }
    }
}
