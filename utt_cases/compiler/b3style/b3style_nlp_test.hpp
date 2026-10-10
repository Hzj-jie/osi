#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include "../test_data_loader.hpp"
#include "../../../compiler/b3style/b3style.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace osi::compiler::b3style_compiler
{
    class b3style_nlp_test : public icase
    {
    private:
        static std::string read_file(const std::filesystem::path& p)
        {
            std::ifstream ifs(p);
            assert(ifs.is_open());
            return std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        }

        static std::vector<std::filesystem::path> sorted_txt_files(const std::filesystem::path& dir)
        {
            std::vector<std::filesystem::path> files;
            for (const auto& entry : std::filesystem::directory_iterator(dir))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".txt")
                {
                    files.push_back(entry.path());
                }
            }
            std::sort(files.begin(), files.end());
            return files;
        }

        void run_dialect_cases(const std::string& dialect)
        {
            static const std::unordered_set<std::string> ignored_compile_errors = {
                "class-initializer-for-non-class.txt",
                "cycle-typedef.txt",
                "duplicate_template_type_parameters.txt",
                "function-return-struct-type-mismatch.txt",
                "reinterpret_cast_heap_with_index.txt",
                "reinterpret_cast_unknown_type.txt",
                "reinterpret_cast_unknown_variable.txt",
                "reinterpret_cast_without_type_id.txt",
                "undefined-value-clause.txt",
                "value-clause-struct-type-mismatch.txt",
                "value-clause-struct-type-mismatch2.txt"
            };

            auto base_dir = test_data::resolve_test_cases_dir(dialect);
            for (const auto& p : sorted_txt_files(base_dir))
            {
                utt_assert.is_true(b3style::nlp()->parse(read_file(p)));
            }

            auto errors_dir = base_dir / "errors";
            for (const auto& p : sorted_txt_files(errors_dir))
            {
                std::string name = p.filename().string();
                if (ignored_compile_errors.find(name) == ignored_compile_errors.end())
                {
                    utt_assert.is_false(b3style::nlp()->parse(read_file(p)));
                }
                else
                {
                    utt_assert.is_true(b3style::nlp()->parse(read_file(p)));
                }
            }
        }

        void run_self_tests()
        {
            for (const auto& p : sorted_txt_files(test_data::utt_cases_dir() / "b2style" / "self-test"))
            {
                auto guard = b3style::parse_wrapper::with_current_file(p.filename().string());
                utt_assert.is_true(b3style::nlp()->parse(read_file(p)));
            }
        }

        void run_self_compile_error_tests()
        {
            static const std::unordered_set<std::string> ignored_compile_errors = {
                "copy-struct-to-primitive.txt",
                "copy-primitive-to-struct.txt",
                "duplicate-var-name-in-inherited-class.txt",
                "duplicate-func-name-in-class.txt",
                "struct-with-myself.txt",
                "templates-with-same-name.txt",
                "type-parameters-with-same-name.txt",
                "mismatch_constructor_parameters.txt"
            };

            for (const auto& p : sorted_txt_files(test_data::utt_cases_dir() / "b2style" / "self-compile-error-test"))
            {
                std::string name = p.filename().string();
                auto guard = b3style::parse_wrapper::with_current_file(name);
                if (ignored_compile_errors.find(name) == ignored_compile_errors.end())
                {
                    utt_assert.is_false(b3style::nlp()->parse(read_file(p)));
                }
                else
                {
                    utt_assert.is_true(b3style::nlp()->parse(read_file(p)));
                }
            }
        }

    public:
        bool run() override
        {
            run_dialect_cases("bstyle");
            run_dialect_cases("b2style");
            run_self_tests();
            run_self_compile_error_tests();
            return true;
        }

        DEFINE_CASE(b3style_nlp_test);
    };

    REGISTER_CASE(b3style_nlp_test);
}
