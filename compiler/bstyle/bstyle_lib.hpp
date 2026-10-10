#pragma once
#include <string>
#include <filesystem>
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        inline std::filesystem::path resolve_compiler_subdir(const std::string& subdir)
        {
            namespace fs = std::filesystem;
            fs::path from_header = fs::path(__FILE__).parent_path().parent_path() / subdir;
            if (fs::exists(from_header))
            {
                return fs::canonical(from_header);
            }
            try
            {
                fs::path p = fs::canonical("/proc/self/exe").parent_path();
                while (!p.empty() && p != p.root_path())
                {
                    if (fs::exists(p / "compiler" / subdir))
                    {
                        return fs::canonical(p / "compiler" / subdir);
                    }
                    p = p.parent_path();
                }
            }
            catch (...) {}

            fs::path cwd = fs::current_path();
            while (!cwd.empty() && cwd != cwd.root_path())
            {
                if (fs::exists(cwd / "compiler" / subdir))
                {
                    return fs::canonical(cwd / "compiler" / subdir);
                }
                cwd = cwd.parent_path();
            }
            assert(false);
            return from_header;
        }

        namespace bstyle_compiler
        {
            class bstyle_lib
            {
            private:
                static const std::filesystem::path& dir()
                {
                    static const std::filesystem::path d = resolve_compiler_subdir("bstyle");
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

                static const std::string& inc_folder()
                {
                    static const std::string folder = (dir() / "lib").string();
                    return folder;
                }
            };
        }
    }
}
