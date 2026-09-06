#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include "../../../compiler/b2style/b2style.hpp"
#include "../../../interpreter/primitive/simulator.hpp"
#include "../../../interpreter/primitive/interrupts.hpp"
#include "../../../interpreter/primitive/console_io.hpp"

namespace fs = std::filesystem;
using namespace osi::compiler::b2style_compiler;

static std::string trim(const std::string& s)
{
    size_t start = 0;
    while (start < s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n'))
    {
        start++;
    }
    size_t end = s.size();
    while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t' || s[end - 1] == '\r' || s[end - 1] == '\n'))
    {
        end--;
    }
    return s.substr(start, end - start);
}

static std::vector<std::string> split_lines(const std::string& s)
{
    std::vector<std::string> lines;
    std::istringstream iss(s);
    std::string line;
    while (std::getline(iss, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return lines;
}

static fs::path find_directory(const std::string& name)
{
    // Try relative to executable
    try
    {
        auto exe_path = fs::canonical("/proc/self/exe").parent_path();
        if (fs::exists(exe_path / name))
        {
            return exe_path / name;
        }
    }
    catch (...)
    {
    }

    if (fs::exists(name))
    {
        return fs::path(name);
    }

    if (fs::exists("tests/compiler/b2style/" + name))
    {
        return fs::path("tests/compiler/b2style/" + name);
    }

    return fs::path();
}

static bool test_self_assertions(int& passed, int& failed, int& skipped)
{
    fs::path self_test_dir = find_directory("self-test");
    if (self_test_dir.empty())
    {
        std::cerr << "Could not find self-test directory!\n";
        return false;
    }

    std::vector<std::string> ignored = {
        "delegate_in_class.txt",
        "delegate_in_class_on_heap.txt",
        "struct-and-primitive-type-with-same-name.txt"
    };

    std::vector<fs::path> entries;
    for (const auto& entry : fs::directory_iterator(self_test_dir))
    {
        if (entry.path().extension() == ".txt")
        {
            entries.push_back(entry.path());
        }
    }
    std::sort(entries.begin(), entries.end());

    std::cout << "Running b2style self-assertion tests (" << entries.size() << " files)...\n";

    for (const auto& path : entries)
    {
        std::string filename = path.filename().string();
        bool is_ign = false;
        for (const auto& ign : ignored)
        {
            if (filename == ign) { is_ign = true; break; }
        }
        if (is_ign)
        {
            std::cout << "[SKIP] " << filename << "\n";
            skipped++;
            continue;
        }

        std::ifstream ifs(path);
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        primitive::console_io::test_wrapper io;
        primitive::simulator sim;
        auto guard = b2style::compile_wrapper::with_current_file(filename);
        bool compile_ok = b2style::with_functions(primitive::interrupts(io.io())).compile(content, sim);
        if (!compile_ok)
        {
            std::cerr << "[FAIL COMPILE] " << filename << "\n";
            failed++;
            continue;
        }

        sim.execute();
        if (sim.halt())
        {
            std::cerr << "[FAIL EXEC] " << filename << " error: " << sim.halt_error() << "\n";
            failed++;
            continue;
        }

        std::string out = trim(io.output());
        const std::string total_assertions_prefix = "Total assertions: ";
        const std::string success_prefix = "Success: ";

        if (!out.empty())
        {
            auto lines = split_lines(out);
            if (!lines.empty() && lines.back().rfind(total_assertions_prefix, 0) == 0)
            {
                uint32_t exp_assertions = std::stoul(lines.back().substr(total_assertions_prefix.size()));
                assert(exp_assertions == lines.size() - 1);
                for (size_t i = 0; i < lines.size() - 1; ++i)
                {
                    assert(lines[i].rfind(success_prefix, 0) == 0);
                }
            }
        }

        std::cout << "[PASS] " << filename << "\n";
        passed++;
    }

    return true;
}

static bool test_self_compile_errors(int& passed, int& failed)
{
    fs::path err_dir = find_directory("self-compile-error-test");
    if (err_dir.empty())
    {
        std::cerr << "Could not find self-compile-error-test directory!\n";
        return false;
    }

    std::vector<fs::path> entries;
    for (const auto& entry : fs::directory_iterator(err_dir))
    {
        if (entry.path().extension() == ".txt")
        {
            entries.push_back(entry.path());
        }
    }
    std::sort(entries.begin(), entries.end());

    std::cout << "\nRunning b2style self-compile-error tests (" << entries.size() << " files)...\n";

    for (const auto& path : entries)
    {
        std::string filename = path.filename().string();
        std::ifstream ifs(path);
        std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

        primitive::simulator sim;
        auto guard = b2style::compile_wrapper::with_current_file(filename);
        bool compile_ok = b2style::with_default_functions().compile(content, sim);
        if (compile_ok)
        {
            std::cerr << "[UNEXPECTED COMPILE SUCCESS] " << filename << "\n";
            failed++;
        }
        else
        {
            std::cout << "[PASS EXPECTED FAIL] " << filename << "\n";
            passed++;
        }
    }

    return true;
}

int main()
{
    int pass_count = 0;
    int fail_count = 0;
    int skip_count = 0;

    if (!test_self_assertions(pass_count, fail_count, skip_count))
    {
        return 1;
    }

    int err_pass_count = 0;
    int err_fail_count = 0;
    if (!test_self_compile_errors(err_pass_count, err_fail_count))
    {
        return 1;
    }

    std::cout << "\n=== b2style Self-Test Summary ===\n";
    std::cout << "Self-assertion tests: " << pass_count << " passed, " << fail_count << " failed, " << skip_count << " skipped.\n";
    std::cout << "Compile-error tests: " << err_pass_count << " passed, " << err_fail_count << " failed.\n";

    if (fail_count > 0 || err_fail_count > 0)
    {
        return 1;
    }

    std::cout << "\nALL b2style SELF-TESTS PASSED!\n";
    return 0;
}
