#pragma once
#include <string>
#include <vector>
#include "instruction_gen.hpp"
#include "variable.hpp"
#include "../../app_info/assert.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class data_ref_operator : public instruction_gen
            {
            protected:
                std::vector<std::string> vs;

                explicit data_ref_operator(std::vector<std::string> vs) : vs(std::move(vs))
                {
                    assert(!this->vs.empty());
                }

                virtual bool variable_restrict(uint32_t i, const variable& v) const = 0;
                virtual std::string instruction_name() const = 0;

            public:
                bool build(std::vector<std::string>& o) const override
                {
                    std::vector<variable> vars(vs.size());
                    for (size_t i = 0; i < vs.size(); ++i)
                    {
                        if (!variable::of(vs[i], o, vars[i]) || !variable_restrict(static_cast<uint32_t>(i), vars[i]))
                        {
                            return false;
                        }
                    }
                    std::string line = instruction_name();
                    for (const auto& v : vars)
                    {
                        line += " ";
                        line += v.ToString();
                    }
                    o.push_back(std::move(line));
                    return true;
                }
            };
        }
    }
}
