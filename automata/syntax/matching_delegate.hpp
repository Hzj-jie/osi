#pragma once
#include <cstdint>
#include <memory>
#include "matching.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class matching_delegate : public matching
            {
            private:
                uint32_t type_;

            public:
                matching_delegate(syntax_collection& col, uint32_t type)
                    : matching(col), type_(type)
                {
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override;
            };
        }
    }
}
