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

            class if_wrapped : public code_gen<logic_writer>
            {
            private:
                std::function<bool(const std::string&)> bypass_;

            public:
                explicit if_wrapped(std::function<bool(const std::string&)> bypass)
                    : bypass_(std::move(bypass))
                {
                    assert(bypass_ != nullptr);
                }

                static std::shared_ptr<if_wrapped> ifdef_wrapped()
                {
                    return std::make_shared<if_wrapped>([](const std::string& s) {
                        return !scope::current()->defines().is_defined(s);
                    });
                }

                static std::shared_ptr<if_wrapped> ifndef_wrapped()
                {
                    return std::make_shared<if_wrapped>([](const std::string& s) {
                        return scope::current()->defines().is_defined(s);
                    });
                }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 3);
                    auto defines_node = n->child(1);
                    for (uint32_t i = 0; i < defines_node->child_count(); ++i)
                    {
                        std::string s;
                        if (i == defines_node->child_count() - 1)
                        {
                            s = defines_node->last_child()->word().str();
                        }
                        else
                        {
                            s = defines_node->child(i)->child(0)->word().str();
                        }
                        if (!bypass_(s))
                        {
                            for (uint32_t j = 2; j <= n->child_count() - 3; ++j)
                            {
                                if (!code_gen_of(n->child(j)).build(o))
                                {
                                    return false;
                                }
                            }
                            auto second_last = n->child(n->child_count() - 2);
                            return (second_last->type_name == "delse-wrapped") ||
                                   code_gen_of(second_last).build(o);
                        }
                    }
                    auto second_last = n->child(n->child_count() - 2);
                    if (second_last->type_name == "delse-wrapped")
                    {
                        for (uint32_t j = 1; j < second_last->child_count(); ++j)
                        {
                            if (!code_gen_of(second_last->child(j)).build(o))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                    return true;
                }
            };

            class define_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "define"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer&) override
                {
                    assert(n != nullptr && n->child_count() == 2);
                    scope::current()->defines().define(n->child(1)->word().str());
                    return true;
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

            inline bool parse_include_file(const std::string& file, logic_writer& o)
            {
                if (file.empty())
                {
                    // Already included
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

            class include_with_string_node : public include_with_string<logic_writer, bstyle_should_include, bstyle_folders>
            {
            public:
                static const char* node_name() { return "include-with-string"; }

            protected:
                bool file_parse(const std::string& s, logic_writer& o) override
                {
                    return parse_include_file(s, o);
                }
            };

            class include_with_file_node : public include_with_file<logic_writer, bstyle_should_include, bstyle_folders>
            {
            public:
                static const char* node_name() { return "include-with-file"; }

            protected:
                bool file_parse(const std::string& s, logic_writer& o) override
                {
                    return parse_include_file(s, o);
                }
            };
        }
    }
}
