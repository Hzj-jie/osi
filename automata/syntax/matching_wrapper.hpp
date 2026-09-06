#pragma once
#include <memory>
#include <vector>
#include "matching.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class matching_wrapper : public matching
            {
            protected:
                std::shared_ptr<matching> m_;

            public:
                matching_wrapper(syntax_collection& col, std::shared_ptr<matching> m)
                    : matching(col), m_(std::move(m))
                {
                    assert(m_ != nullptr);
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    return m_->match(v, p);
                }

                const std::shared_ptr<matching>& inner() const { return m_; }
            };
        }
    }
}
