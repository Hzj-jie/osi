#pragma once
#include <string>
#include "data_ref_operator.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class ternary_operator : public data_ref_operator
            {
            protected:
                ternary_operator(std::string result, std::string p1, std::string p2, std::string p3)
                    : data_ref_operator({std::move(result), std::move(p1), std::move(p2), std::move(p3)}) {}

                virtual bool result_restrict(const variable& result) const = 0;
                virtual bool parameter1_restrict(const variable& p1) const { return parameter_restrict(p1); }
                virtual bool parameter2_restrict(const variable& p2) const { return parameter_restrict(p2); }
                virtual bool parameter3_restrict(const variable& p3) const { return parameter_restrict(p3); }
                virtual bool parameter_restrict(const variable&) const { return true; }

                bool variable_restrict(uint32_t i, const variable& v) const override
                {
                    if (i == 0) return result_restrict(v);
                    if (i == 1) return parameter1_restrict(v);
                    if (i == 2) return parameter2_restrict(v);
                    if (i == 3) return parameter3_restrict(v);
                    assert(false);
                    return false;
                }
            };
        }
    }
}
