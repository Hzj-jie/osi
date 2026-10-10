#pragma once
#include <vector>
#include "../logic/builders.hpp"

namespace osi
{
    namespace compiler
    {
        class params_t
        {
        private:
            std::vector<parameter> v_;

        public:
            void pack(const std::vector<parameter>& n)
            {
                v_.insert(v_.end(), n.begin(), n.end());
            }

            std::vector<parameter> unpack()
            {
                std::vector<parameter> res = std::move(v_);
                v_.clear();
                return res;
            }
        };
    }
}
