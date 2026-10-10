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
#include "../b3style_forward.hpp"
#include "../../bstyle/nodes/preprocessor.hpp"
#include "../../b2style/nodes/preprocessor.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b3style_compiler
        {
            using current_file_guard = bstyle_compiler::current_file_guard;

            struct b3style_folders
            {
                static std::vector<std::string>& custom_folders()
                {
                    static std::vector<std::string> f;
                    return f;
                }

                std::vector<std::string> operator()() const
                {
                    std::vector<std::string> r;
                    r.push_back(b2style_compiler::b2style_inc_folder());
                    r.push_back(bstyle_compiler::bstyle_inc_folder());
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

            struct b3style_should_include
            {
                bool operator()(const std::string& s) const
                {
                    return scope::current()->includes().should_include(s);
                }
            };
        }
    }
}

#define DIALECT b3style_compiler
#define WRITER logic_writer
#define PREPROCESSOR_FOLDERS b3style_folders
#define PREPROCESSOR_SHOULD_INCLUDE b3style_should_include
#include "../../bstyle/nodes/preprocessor.inc"
#undef PREPROCESSOR_SHOULD_INCLUDE
#undef PREPROCESSOR_FOLDERS
#undef WRITER
#undef DIALECT
