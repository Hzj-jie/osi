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
            class matching_group : public matching
            {
            private:
                std::vector<std::shared_ptr<matching>> ms_;

            public:
                matching_group(syntax_collection& col, std::vector<std::shared_ptr<matching>> ms)
                    : matching(col), ms_(std::move(ms))
                {
                    assert(!ms_.empty());
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    if (v == nullptr || p >= v->size())
                    {
                        return result::failure(p);
                    }
                    result r = result::failure(p);
                    for (size_t i = 0; i < ms_.size(); ++i)
                    {
                        assert(ms_[i] != nullptr);
                        result c = ms_[i]->match(v, p);
                        r = r | c;
                    }
                    return r;
                }
            };
        }
    }
}
