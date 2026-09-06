#pragma once
#include <memory>
#include "matching_wrapper.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class optional_matching_group : public matching_wrapper
            {
            public:
                optional_matching_group(syntax_collection& col, std::shared_ptr<matching> m)
                    : matching_wrapper(col, std::move(m))
                {
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    result r = m_->match(v, p);
                    if (r.succeeded())
                    {
                        return r;
                    }
                    return result::success(p) | r;
                }
            };
        }
    }
}
