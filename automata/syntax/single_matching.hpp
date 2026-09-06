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
            class single_matching : public matching
            {
            private:
                uint32_t m_;

            public:
                single_matching(syntax_collection& col, uint32_t m)
                    : matching(col), m_(m)
                {
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    if (v == nullptr || p >= v->size())
                    {
                        return result::failure(p);
                    }
                    if ((*v)[p].type != m_)
                    {
                        return result::failure(p);
                    }
                    auto node = create_node(v, (*v)[p].type, p, p + 1);
                    return result::success(p + 1, node);
                }
            };
        }
    }
}
