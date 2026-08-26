#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../interpreter/primitive/simulator.hpp"
#include "../../interpreter/primitive/parser.hpp"
#include "../../interpreter/primitive/interrupts.hpp"
#include "../../app_info/assert.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class instruction_gen
            {
            public:
                virtual ~instruction_gen() = default;
                virtual bool build(std::vector<std::string>& o) const = 0;
            };
        }
    }
}
