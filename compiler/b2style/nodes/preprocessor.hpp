#pragma once
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>
#include "../scope.hpp"
#include "../b2style_forward.hpp"
#include "../b2style_lib.hpp"
#include "../../rewriter/typed_node_writer.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../code_gen/includes.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../bstyle/nodes/preprocessor.hpp"

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {
            inline const std::string& b2style_inc_folder()
            {
                return b2style_lib::inc_folder();
            }

            struct b2style_folders
            {
                std::vector<std::string> operator()() const
                {
                    std::vector<std::string> r;
                    r.push_back(b2style_inc_folder());
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
                    return r;
                }
            };
        }
    }
}

#define DIALECT b2style_compiler
#define WRITER rewriter::typed_node_writer
#define PREPROCESSOR_FOLDERS b2style_folders
#include "../../bstyle/nodes/preprocessor.inc"
#undef PREPROCESSOR_FOLDERS
#undef WRITER
#undef DIALECT
