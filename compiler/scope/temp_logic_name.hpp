#pragma once
#include <string>
#include <stdint.h>

namespace osi
{
    namespace compiler
    {
        class temp_logic_name_t
        {
        private:
            uint32_t id_{0};

        public:
            std::string variable()
            {
                id_++;
                return "temp_value_@" + std::to_string(id_);
            }

            std::string logic()
            {
                id_++;
                return "temp_logic_@" + std::to_string(id_);
            }
        };
    }
}
