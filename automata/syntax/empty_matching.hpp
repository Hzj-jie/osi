#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "matching.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class empty_matching : public matching
            {
            public:
                explicit empty_matching(syntax_collection& col) : matching(col) {}

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    if (v != nullptr && p < v->size())
                    {
                        return result::success(p);
                    }
                    return result::failure(p);
                }
            };
        }
    }
}
