#pragma once
#include <string>
#include "../b2style/b2style_lib.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class b3style_lib
            {
            public:
                static const std::string& nlexer_rule()
                {
                    return b2style_compiler::b2style_lib::nlexer_rule();
                }

                static const std::string& syntaxer_rule()
                {
                    return b2style_compiler::b2style_lib::syntaxer_rule();
                }
            };
        }
    }
}
