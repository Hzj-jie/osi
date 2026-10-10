#pragma once
#include <string>
#include <unordered_set>

namespace osi
{
    namespace compiler
    {
        class define_t
        {
        private:
            std::unordered_set<std::string> d_;

        public:
            void define(const std::string& s)
            {
                d_.insert(s);
            }

            bool is_defined(const std::string& s) const
            {
                return d_.find(s) != d_.end();
            }
        };
    }
}
