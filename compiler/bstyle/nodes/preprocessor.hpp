#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../code_gen/includes.hpp"
#include "../../logic/logic_writer.hpp"
#include "../scope.hpp"
#include "../bstyle_lib.hpp"
#include "../bstyle_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class current_file_guard
            {
            private:
                static std::vector<std::string>& stack()
                {
                    static thread_local std::vector<std::string> s;
                    return s;
                }
            public:
                static std::string get()
                {
                    if (!stack().empty())
                    {
                        return stack().back();
                    }
                    return "unknown_file";
                }

                explicit current_file_guard(const std::string& file)
                {
                    stack().push_back(file);
                }

                ~current_file_guard()
                {
                    if (!stack().empty())
                    {
                        stack().pop_back();
                    }
                }
            };

            inline const std::string& bstyle_inc_folder()
            {
                static std::string folder = []() {
                    std::string p = std::filesystem::temp_directory_path().string() + "/osi_bstyle_inc";
                    std::filesystem::create_directories(p);
                    for (const auto& kv : bstyle_lib::files())
                    {
                        std::filesystem::path fp = std::filesystem::path(p) / kv.first;
                        std::filesystem::create_directories(fp.parent_path());
                        std::ofstream ofs(fp);
                        ofs << kv.second;
                    }
                    return p;
                }();
                return folder;
            }

            struct bstyle_folders
            {
                static std::vector<std::string>& custom_folders()
                {
                    static std::vector<std::string> f;
                    return f;
                }

                std::vector<std::string> operator()() const
                {
                    std::vector<std::string> r;
                    r.push_back(bstyle_inc_folder());
                    std::string cur = current_file_guard::get();
                    if (!cur.empty() && cur != "unknown_file")
                    {
                        std::filesystem::path cp(cur);
                        if (cp.has_parent_path())
                        {
                            r.push_back(cp.parent_path().string());
                        }
                    }
                    r.push_back(".");
                    for (const auto& cf : custom_folders())
                    {
                        r.push_back(cf);
                    }
                    return r;
                }
            };

            struct bstyle_should_include
            {
                bool operator()(const std::string& s) const
                {
                    return scope::current()->includes().should_include(s);
                }
            };
        }
    }
}

#define DIALECT bstyle_compiler
#define WRITER logic_writer
#define PREPROCESSOR_FOLDERS bstyle_folders
#define PREPROCESSOR_SHOULD_INCLUDE bstyle_should_include
#include "preprocessor.inc"
#undef PREPROCESSOR_SHOULD_INCLUDE
#undef PREPROCESSOR_FOLDERS
#undef WRITER
#undef DIALECT
