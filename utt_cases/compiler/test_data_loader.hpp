#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include <iterator>
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        namespace test_data
        {
            inline std::filesystem::path resolve_test_cases_dir(const std::string& dialect)
            {
                namespace fs = std::filesystem;
                fs::path from_header = fs::path(__FILE__).parent_path() / dialect / "cases";
                if (fs::exists(from_header))
                {
                    return fs::canonical(from_header);
                }
                try
                {
                    fs::path p = fs::canonical("/proc/self/exe").parent_path();
                    while (!p.empty() && p != p.root_path())
                    {
                        if (fs::exists(p / "utt_cases" / "compiler" / dialect / "cases"))
                        {
                            return fs::canonical(p / "utt_cases" / "compiler" / dialect / "cases");
                        }
                        p = p.parent_path();
                    }
                }
                catch (...) {}

                fs::path cwd = fs::current_path();
                while (!cwd.empty() && cwd != cwd.root_path())
                {
                    if (fs::exists(cwd / "utt_cases" / "compiler" / dialect / "cases"))
                    {
                        return fs::canonical(cwd / "utt_cases" / "compiler" / dialect / "cases");
                    }
                    cwd = cwd.parent_path();
                }
                assert(false);
                return from_header;
            }

            inline std::string load(const std::string& dialect, const std::string& rel_path)
            {
                std::filesystem::path p = resolve_test_cases_dir(dialect) / rel_path;
                std::ifstream ifs(p);
                assert(ifs.is_open());
                return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            }
        }
    }
}
