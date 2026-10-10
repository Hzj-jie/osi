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
#include "../../../automata/typed_node.hpp"
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../bstyle/bstyle_lib.hpp"



#define DIALECT b2style_compiler
#define WRITER rewriter::typed_node_writer
#include "../../bstyle/nodes/preprocessor.inc"
#undef WRITER
#undef DIALECT

namespace osi
{
    namespace compiler
    {
        namespace b2style_compiler
        {

            inline const std::string& b2style_inc_folder()
            {
                static std::string folder = []() {
                    std::string p = std::filesystem::temp_directory_path().string() + "/osi_b2style_inc";
                    std::filesystem::create_directories(p);
                    for (const auto& kv : b2style_lib::files())
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

            inline const std::string& bstyle_inc_folder_ref()
            {
                static std::string folder = []() {
                    std::string p = std::filesystem::temp_directory_path().string() + "/osi_bstyle_inc";
                    std::filesystem::create_directories(p);
                    for (const auto& kv : bstyle_compiler::bstyle_lib::files())
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

            struct b2style_folders
            {
                std::vector<std::string> operator()() const
                {
                    std::vector<std::string> r;
                    r.push_back(b2style_inc_folder());
                    r.push_back(bstyle_inc_folder_ref());
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

            inline bool parse_include_file(const std::string& file, rewriter::typed_node_writer& o)
            {
                if (file.empty())
                {
                    return true;
                }
                std::ifstream ifs(file);
                if (!ifs.is_open())
                {
                    raise_error("Cannot read content from ", file);
                    return false;
                }
                std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                current_file_guard guard(file);
                return build_code(content, o);
            }

            class include_with_string_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "include-with-string"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    std::string rel_path = n->child(1)->word().str();
                    if (rel_path.size() >= 2 && rel_path.front() == '"' && rel_path.back() == '"')
                    {
                        rel_path = rel_path.substr(1, rel_path.size() - 2);
                    }

                    b2style_folders folders;
                    std::string full_path;
                    for (const auto& dir : folders())
                    {
                        std::filesystem::path p = std::filesystem::path(dir) / rel_path;
                        if (std::filesystem::exists(p))
                        {
                            full_path = std::filesystem::canonical(p).string();
                            break;
                        }
                    }

                    if (full_path.empty())
                    {
                        raise_error("Cannot find include file ", rel_path);
                        return false;
                    }

                    if (!scope::current()->includes().should_include(full_path))
                    {
                        return true;
                    }
                    return parse_include_file(full_path, o);
                }
            };

            class include_with_file_node : public code_gen<rewriter::typed_node_writer>
            {
            public:
                static const char* node_name() { return "include-with-file"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, rewriter::typed_node_writer& o) override
                {
                    assert(n != nullptr);
                    std::string str = n->word().str();
                    size_t lt = str.find('<');
                    size_t gt = str.rfind('>');
                    if (lt == std::string::npos || gt == std::string::npos || lt >= gt)
                    {
                        raise_error("Invalid include-with-file: ", str);
                        return false;
                    }
                    std::string rel_path = str.substr(lt + 1, gt - lt - 1);

                    b2style_folders folders;
                    std::string full_path;
                    for (const auto& dir : folders())
                    {
                        std::filesystem::path p = std::filesystem::path(dir) / rel_path;
                        if (std::filesystem::exists(p))
                        {
                            full_path = std::filesystem::canonical(p).string();
                            break;
                        }
                    }

                    if (full_path.empty())
                    {
                        raise_error("Cannot find include file <", rel_path, ">");
                        return false;
                    }

                    if (!scope::current()->includes().should_include(full_path))
                    {
                        return true;
                    }
                    return parse_include_file(full_path, o);
                }
            };
        }
    }
}
