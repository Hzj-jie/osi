#pragma once
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "reparser.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../../utils/strutils.hpp"

namespace osi
{
    namespace compiler
    {
        inline std::string trim(const std::string& s, const std::string& drop = " \t\r\n")
        {
            size_t start = s.find_first_not_of(drop);
            if (start == std::string::npos) return "";
            size_t end = s.find_last_not_of(drop);
            return s.substr(start, end - start + 1);
        }

        inline std::string c_unescape(const std::string& s)
        {
            std::string res;
            res.reserve(s.size());
            for (size_t i = 0; i < s.size(); ++i)
            {
                if (s[i] == '\\' && i + 1 < s.size())
                {
                    char next = s[++i];
                    switch (next)
                    {
                        case 'a': res += '\a'; break;
                        case 'b': res += '\b'; break;
                        case 'f': res += '\f'; break;
                        case 'n': res += '\n'; break;
                        case 'r': res += '\r'; break;
                        case 't': res += '\t'; break;
                        case 'v': res += '\v'; break;
                        case '\\': res += '\\'; break;
                        case '\'': res += '\''; break;
                        case '\"': res += '\"'; break;
                        case '?': res += '\?'; break;
                        case '0': res += '\0'; break;
                        default: res += next; break;
                    }
                }
                else
                {
                    res += s[i];
                }
            }
            return res;
        }

        template <typename Writer, typename ShouldIncludeFunc, typename FoldersFunc>
        class includes : public reparser<Writer>
        {
        protected:
            bool parse(const std::string& s, Writer& o) override
            {
                return file_parse(s, o);
            }

            virtual bool file_parse(const std::string& s, Writer& o) = 0;

            static bool check_include_file(const std::string& p, const std::string& s, std::string& o)
            {
                std::filesystem::path fp = std::filesystem::path(p) / s;
                if (std::filesystem::exists(fp))
                {
                    o = fp.string();
                    return true;
                }
                return false;
            }

            static bool check_include_file(const std::vector<std::string>& v, const std::string& s, std::string& o)
            {
                for (const auto& folder : v)
                {
                    if (check_include_file(folder, s, o))
                    {
                        return true;
                    }
                }
                return false;
            }

            bool resolve_include_file(const std::string& s, std::string& o)
            {
                if (!ShouldIncludeFunc{}(s))
                {
                    // Already included
                    o = "";
                    return true;
                }
                std::vector<std::string> folders = FoldersFunc{}();
                if (check_include_file(folders, s, o))
                {
                    return true;
                }
                raise_error("Cannot find include file ", s);
                return false;
            }
        };

        template <typename Writer, typename ShouldIncludeFunc, typename FoldersFunc>
        class include_with_string : public includes<Writer, ShouldIncludeFunc, FoldersFunc>
        {
        protected:
            bool dump(const std::shared_ptr<automata::typed_node>& n, std::string& o) override
            {
                assert(n != nullptr);
                assert(n->child_count() == 2);
                std::string s = n->child(1)->word().str();
                s = trim(s, "\"");
                s = c_unescape(s);
                return this->resolve_include_file(s, o);
            }
        };

        template <typename Writer, typename ShouldIncludeFunc, typename FoldersFunc>
        class include_with_file : public includes<Writer, ShouldIncludeFunc, FoldersFunc>
        {
        protected:
            bool dump(const std::shared_ptr<automata::typed_node>& n, std::string& o) override
            {
                assert(n != nullptr);
                assert(n->leaf());
                std::string file = n->word().str();
                const std::string kw_include = "#include";
                assert(file.rfind(kw_include, 0) == 0);
                file = trim(file.substr(kw_include.length()));
                assert(file.front() == '<' && file.back() == '>');
                file = file.substr(1, file.length() - 2);
                return this->resolve_include_file(file, o);
            }
        };
    }
}
