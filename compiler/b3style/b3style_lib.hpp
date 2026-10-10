#pragma once
#include <string>
#include <filesystem>
#include "../bstyle/bstyle_lib.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            class b3style_lib
            {
            private:
                static const std::filesystem::path& dir()
                {
                    static const std::filesystem::path d = resolve_compiler_subdir("b3style");
                    return d;
                }

            public:
                static const std::string& nlexer_rule()
                {
                    static const std::string rule = (dir() / "nlexer_rule.txt").string();
                    return rule;
                }

                static const std::string& syntaxer_rule()
                {
                    static const std::string rule = (dir() / "syntaxer_rule.txt").string();
                    return rule;
                }
            };
        }
    }
}
