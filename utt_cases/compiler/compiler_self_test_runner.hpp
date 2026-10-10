#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include "test_data_loader.hpp"
#include "../../interpreter/primitive/simulator.hpp"
#include "../../interpreter/primitive/console_io.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace osi::compiler
{
    class b2style_self_test_runner : public icase
    {
    protected:
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

        static std::filesystem::path find_directory(const std::string& name)
        {
            return test_data::utt_cases_dir() / "b2style" / name;
        }

        virtual bool compile_with_io(primitive::console_io::test_wrapper& io,
                                     const std::string& filename,
                                     const std::string& content,
                                     primitive::simulator& sim) = 0;

        virtual bool compile_default(const std::string& filename,
                                     const std::string& content,
                                     primitive::simulator& sim) = 0;

        virtual bool ignore_assertion_case(const std::string& name) const
        {
            return name == "delegate_in_class.txt" ||
                   name == "delegate_in_class_on_heap.txt" ||
                   name == "struct-and-primitive-type-with-same-name.txt";
        }

        virtual bool ignore_compile_error_case(const std::string&) const
        {
            return false;
        }

        bool test_self_assertions(int& passed, int& failed, int& skipped)
        {
            namespace fs = std::filesystem;
            fs::path self_test_dir = find_directory("self-test");
            if (self_test_dir.empty())
            {
                std::cerr << "Could not find self-test directory!\n";
                return false;
            }

            std::vector<fs::path> entries;
            for (const auto& entry : fs::directory_iterator(self_test_dir))
            {
                if (entry.path().extension() == ".txt")
                {
                    entries.push_back(entry.path());
                }
            }
            std::sort(entries.begin(), entries.end());

            for (const auto& path : entries)
            {
                std::string filename = path.filename().string();
                if (ignore_assertion_case(filename))
                {
                    std::cout << "[SKIP] " << filename << "\n";
                    skipped++;
                    continue;
                }

                std::ifstream ifs(path);
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

                primitive::console_io::test_wrapper io;
                primitive::simulator sim;
                if (!compile_with_io(io, filename, content, sim))
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
                        utt_assert.equal(static_cast<size_t>(exp_assertions), lines.size() - 1);
                        for (size_t i = 0; i < lines.size() - 1; ++i)
                        {
                            utt_assert.is_true(lines[i].rfind(success_prefix, 0) == 0);
                        }
                    }
                }

                std::cout << "[PASS] " << filename << "\n";
                passed++;
            }

            return true;
        }

        bool test_self_compile_errors(int& passed, int& failed)
        {
            namespace fs = std::filesystem;
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

            for (const auto& path : entries)
            {
                std::string filename = path.filename().string();
                if (ignore_compile_error_case(filename))
                {
                    std::cout << "[SKIP] " << filename << "\n";
                    continue;
                }
                std::ifstream ifs(path);
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

                primitive::simulator sim;
                if (compile_default(filename, content, sim))
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

    public:
        bool run() override
        {
            int pass_count = 0;
            int fail_count = 0;
            int skip_count = 0;

            utt_assert.is_true(test_self_assertions(pass_count, fail_count, skip_count));
            utt_assert.equal(fail_count, 0);

            int err_pass_count = 0;
            int err_fail_count = 0;
            utt_assert.is_true(test_self_compile_errors(err_pass_count, err_fail_count));
            utt_assert.equal(err_fail_count, 0);

            return fail_count == 0 && err_fail_count == 0;
        }
    };
}
