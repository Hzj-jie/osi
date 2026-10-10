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
            inline std::filesystem::path utt_cases_dir()
            {
                return std::filesystem::canonical(std::filesystem::path(__FILE__).parent_path());
            }

            inline std::filesystem::path resolve_test_cases_dir(const std::string& dialect)
            {
                return utt_cases_dir() / dialect / "cases";
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
