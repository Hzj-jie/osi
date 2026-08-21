#pragma once
#include <string>
#include "instruction_gen.hpp"
#include "variable.hpp"
#include "../../../primitive/data_block.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class _copy_const : public instruction_gen
            {
            private:
                std::string target;
                primitive::data_block data;

            public:
                _copy_const(std::string target, primitive::data_block data)
                    : target(std::move(target)), data(std::move(data))
                {
                    assert(!this->target.empty());
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable t;
                    if (variable::of(target, o, t) && t.is_assignable_from(static_cast<uint32_t>(data.value_bytes_size())))
                    {
                        o.push_back(strcat("cpc ", t.ToString(), " ", data.to_assembly_string()));
                        return true;
                    }
                    return false;
                }
            };
        }
    }
}
