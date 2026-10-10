#pragma once
#include <string>
#include <unordered_set>

namespace osi
{
    namespace compiler
    {
        class includes_t
        {
        private:
            std::unordered_set<std::string> included_;

        public:
            bool should_include(const std::string& f)
            {
                return included_.insert(f).second;
            }
        };
    }
}
