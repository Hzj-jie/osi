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
            class multi_matching_group : public matching_wrapper
            {
            public:
                multi_matching_group(syntax_collection& col, std::shared_ptr<matching> m)
                    : matching_wrapper(col, std::move(m))
                {
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    std::vector<std::shared_ptr<typed_node>> nodes;
                    result r = result::failure(p);
                    while (true)
                    {
                        result c = m_->match(v, p);
                        r = r | c;
                        if (c.failed())
                        {
                            break;
                        }
                        if (c.suc->pos == p)
                        {
                            break;
                        }
                        p = c.suc->pos;
                        nodes.insert(nodes.end(), c.suc->nodes.begin(), c.suc->nodes.end());
                    }
                    if (nodes.empty())
                    {
                        assert(r.failed());
                        return r;
                    }
                    return result::success(p, std::move(nodes)) | r;
                }
            };
        }
    }
}
