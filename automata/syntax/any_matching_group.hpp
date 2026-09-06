#pragma once
#include <memory>
#include <vector>
#include "matching_wrapper.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class any_matching_group : public matching_wrapper
            {
            public:
                any_matching_group(syntax_collection& col, std::shared_ptr<matching> m)
                    : matching_wrapper(col, std::move(m))
                {
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    std::vector<std::shared_ptr<typed_node>> nodes;
                    result r = m_->match(v, p);
                    while (r.succeeded())
                    {
                        if (r.suc->pos == p)
                        {
                            break;
                        }
                        p = r.suc->pos;
                        nodes.insert(nodes.end(), r.suc->nodes.begin(), r.suc->nodes.end());
                        r = m_->match(v, p);
                    }
                    return result::success(p, std::move(nodes)) | r;
                }
            };
        }
    }
}
