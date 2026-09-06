#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cctype>
#include "../utils/strutils.hpp"
#include "../app_info/assert.hpp"
#include "../app_info/error_handle.hpp"

namespace osi
{
    namespace configuration
    {
        class rule
        {
        public:
            constexpr static char comment_start = '#';
            constexpr static char command_separator = ' ';
            inline static const std::string command_include = "INCLUDE";

        private:
            std::string cur_file;

        protected:
            virtual const std::unordered_map<std::string, std::function<bool(const std::string&)>>& command_mapping() = 0;
            virtual bool default_handler(const std::string& s, const std::string& f) = 0;

            const std::string& current_file() const { return cur_file; }

            virtual bool finish() { return true; }

            bool include(const std::string& path)
            {
                namespace fs = std::filesystem;
                std::string s = path;
                if (!fs::exists(s) && !cur_file.empty())
                {
                    fs::path cur_p(cur_file);
                    s = (cur_p.parent_path() / s).string();
                }
                if (!fs::exists(s))
                {
                    raise_error("INCLUDE file not found: ", path, " (resolved: ", s, ")");
                    return false;
                }
                return parse_file_impl(s);
            }

            bool parse_impl(const std::vector<std::string>& lines)
            {
                for (size_t i = 0; i < lines.size(); ++i)
                {
                    std::string line = lines[i];
                    size_t start = 0;
                    while (start < line.size() && std::isspace(static_cast<unsigned char>(line[start]))) start++;
                    if (start >= line.size()) continue;
                    size_t end = line.size();
                    while (end > start && std::isspace(static_cast<unsigned char>(line[end - 1]))) end--;
                    line = line.substr(start, end - start);

                    if (line.empty() || line[0] == comment_start) continue;

                    std::string f, s;
                    size_t sp = line.find(command_separator);
                    if (sp == std::string::npos)
                    {
                        f = line;
                        s = "";
                    }
                    else
                    {
                        f = line.substr(0, sp);
                        size_t s_start = sp + 1;
                        while (s_start < line.size() && line[s_start] == command_separator) s_start++;
                        s = line.substr(s_start);
                    }

                    if (f.empty()) continue;

                    if (f == command_include)
                    {
                        if (!include(s))
                        {
                            raise_error("failed to execute command ", line);
                            return false;
                        }
                    }
                    else
                    {
                        const auto& mapping = command_mapping();
                        auto it = mapping.find(f);
                        if (it != mapping.end())
                        {
                            if (!it->second(s))
                            {
                                raise_error("failed to execute command ", line);
                                return false;
                            }
                        }
                        else
                        {
                            if (!default_handler(s, f))
                            {
                                raise_error("failed to execute command ", line);
                                return false;
                            }
                        }
                    }
                }
                return true;
            }

            bool parse_file_impl(const std::string& rule_file)
            {
                std::ifstream ifs(rule_file);
                if (!ifs.is_open())
                {
                    raise_error("failed to read rule-file ", rule_file);
                    return false;
                }
                std::vector<std::string> lines;
                std::string line;
                while (std::getline(ifs, line))
                {
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    lines.push_back(std::move(line));
                }

                std::string last_file = cur_file;
                cur_file = rule_file;
                bool ok = parse_impl(lines);
                cur_file = last_file;
                return ok;
            }

        public:
            virtual ~rule() = default;

            bool parse(const std::vector<std::string>& lines)
            {
                return parse_impl(lines) && finish();
            }

            bool parse_file(const std::string& rule_file)
            {
                return parse_file_impl(rule_file) && finish();
            }

            bool parse_content(const std::string& content)
            {
                std::vector<std::string> lines;
                std::istringstream iss(content);
                std::string line;
                while (std::getline(iss, line))
                {
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    lines.push_back(std::move(line));
                }
                return parse(lines);
            }
        };
    }
}
