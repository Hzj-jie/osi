#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <cctype>
#include "statements.hpp"
#include "importer.definition.hpp"
#include "../../interpreter/primitive/simulator.hpp"
#include "../../interpreter/primitive/parser.hpp"
#include "../../interpreter/primitive/data_block.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class importer : private importer_definition
            {
            private:
                std::shared_ptr<primitive::interrupts> functions;

                static std::string strip_comments(std::string_view s)
                {
                    std::string res;
                    res.reserve(s.size());
                    size_t i = 0;
                    while (i < s.size())
                    {
                        if (s[i] == '#' && i + 1 < s.size() && s[i + 1] == '#')
                        {
                            i += 2;
                            while (i < s.size() && s[i] != '\n') i++;
                            if (i < s.size() && s[i] == '\n') res += s[i++];
                        }
                        else
                        {
                            res += s[i++];
                        }
                    }
                    return res;
                }

                static std::vector<std::string> tokenize(std::string_view s)
                {
                    std::string clean = strip_comments(s);
                    std::vector<std::string> tokens;
                    size_t i = 0;
                    while (i < clean.size())
                    {
                        while (i < clean.size() && std::isspace(static_cast<unsigned char>(clean[i]))) i++;
                        if (i >= clean.size()) break;

                        char c = clean[i];
                        if (c == '(' || c == ')' || c == '{' || c == '}')
                        {
                            tokens.emplace_back(1, c);
                            i++;
                        }
                        else
                        {
                            size_t start = i;
                            while (i < clean.size() &&
                                   !std::isspace(static_cast<unsigned char>(clean[i])) &&
                                   clean[i] != '(' && clean[i] != ')' &&
                                   clean[i] != '{' && clean[i] != '}')
                            {
                                i++;
                            }
                            tokens.push_back(clean.substr(start, i - start));
                        }
                    }
                    return tokens;
                }

            public:
                explicit importer(std::shared_ptr<primitive::interrupts> functions = nullptr)
                    : functions(std::move(functions))
                {
                    if (!this->functions) this->functions = std::make_shared<primitive::interrupts>();
                }

                bool import(const std::vector<std::string>& tokens, std::vector<std::shared_ptr<instruction_gen>>& o)
                {
                    size_t p = 0;
                    while (p < tokens.size())
                    {
                        std::shared_ptr<instruction_gen> e;
                        if (!parse(tokens, p, e)) return false;
                        o.push_back(std::move(e));
                    }
                    return true;
                }

                bool import(std::string_view s, std::vector<std::shared_ptr<instruction_gen>>& o)
                {
                    std::vector<std::string> tokens = tokenize(s);
                    if (tokens.empty()) return false;
                    return import(tokens, o);
                }

                static bool compile(const std::vector<std::shared_ptr<instruction_gen>>& es,
                                    std::vector<std::string>& o,
                                    std::shared_ptr<primitive::interrupts> intr = nullptr)
                {
                    o.clear();
                    scope root_scope(intr ? intr : std::make_shared<primitive::interrupts>());
                    for (const auto& e : es)
                    {
                        if (!e || !e->build(o)) return false;
                    }
                    root_scope.disable_end_scope();
                    return true;
                }

                static bool compile_to_instructions(const std::vector<std::shared_ptr<instruction_gen>>& es,
                                                   std::vector<primitive::instruction>& out_insts,
                                                   std::shared_ptr<primitive::interrupts> intr = nullptr)
                {
                    std::vector<std::string> o;
                    if (!compile(es, o, intr)) return false;
                    out_insts.clear();
                    for (const auto& line : o)
                    {
                        if (line.empty()) continue;
                        primitive::instruction inst;
                        if (!primitive::parser::parse_instruction(line, inst))
                        {
                            return false;
                        }
                        out_insts.push_back(std::move(inst));
                    }
                    return true;
                }

                bool import(const std::string& s, primitive::simulator& sim)
                {
                    if (this->functions)
                    {
                        sim.mem().intr() = *this->functions;
                    }
                    std::vector<std::shared_ptr<instruction_gen>> es;
                    if (!import(s, es)) return false;
                    std::vector<primitive::instruction> insts;
                    if (!compile_to_instructions(es, insts, std::shared_ptr<primitive::interrupts>(&sim.mem().intr(), [](void*){})))
                    {
                        return false;
                    }
                    sim.load_instructions(std::move(insts));
                    return true;
                }
            };
        }
    }
}
