#pragma once
#include <string>
#include "data_ref_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class pair_result_binary_operator : public data_ref_operator
            {
            protected:
                pair_result_binary_operator(std::string result1, std::string result2, std::string left, std::string right)
                    : data_ref_operator({std::move(result1), std::move(result2), std::move(left), std::move(right)}) {}

                virtual bool result1_restrict(const variable& r1) const { return result_restrict(r1); }
                virtual bool result2_restrict(const variable& r2) const { return result_restrict(r2); }
                virtual bool result_restrict(const variable&) const { return true; }
                virtual bool left_restrict(const variable& left) const { return parameter_restrict(left); }
                virtual bool right_restrict(const variable& right) const { return parameter_restrict(right); }
                virtual bool parameter_restrict(const variable&) const { return true; }

                bool variable_restrict(uint32_t i, const variable& v) const override
                {
                    if (i == 0) return result1_restrict(v);
                    if (i == 1) return result2_restrict(v);
                    if (i == 2) return left_restrict(v);
                    if (i == 3) return right_restrict(v);
                    assert(false);
                    return false;
                }
            };
        }
    }
}
