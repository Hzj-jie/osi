#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <cctype>
#include "statements.hpp"
#include "../../interpreter/primitive/simulator.hpp"
#include "../../interpreter/primitive/parser.hpp"
#include "../../interpreter/primitive/data_block.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class importer
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

                static bool parse_typed_parameters(std::vector<parameter>& o, const std::vector<std::string>& v, size_t& p)
                {
                    assert(v.size() > p);
                    if (v[p] != "(") return false;
                    p++;
                    while (p < v.size())
                    {
                        if (v[p] == ")")
                        {
                            p++;
                            return true;
                        }
                        if (p + 1 >= v.size()) return false;
                        std::string param_name = v[p];
                        std::string param_type = v[p + 1];
                        o.emplace_back(param_type, param_name);
                        p += 2;
                    }
                    errors::typed_parameters_is_not_closed();
                    return false;
                }

                static bool parse_parameters(std::vector<std::string>& o, const std::vector<std::string>& v, size_t& p)
                {
                    assert(v.size() > p);
                    if (v[p] != "(") return false;
                    p++;
                    while (p < v.size())
                    {
                        if (v[p] == ")")
                        {
                            p++;
                            return true;
                        }
                        o.push_back(v[p]);
                        p++;
                    }
                    errors::parameters_is_not_closed();
                    return false;
                }

                bool parse_paragraph(std::shared_ptr<paragraph>& o, const std::vector<std::string>& v, size_t& p)
                {
                    assert(v.size() > p);
                    if (v[p] != "{") return false;
                    o = std::make_shared<paragraph>();
                    p++;
                    while (p < v.size())
                    {
                        if (v[p] == "}")
                        {
                            p++;
                            return true;
                        }
                        std::shared_ptr<instruction_gen> e;
                        if (!parse(v, p, e)) return false;
                        o->push(std::move(e));
                    }
                    errors::paragraph_is_not_closed();
                    return false;
                }

                bool parse(const std::vector<std::string>& v, size_t& p, std::shared_ptr<instruction_gen>& o)
                {
                    if (p >= v.size()) return false;
                    const std::string& kw = v[p];

                    if (kw == "start_scope")
                    {
                        size_t pos = p + 1;
                        std::shared_ptr<paragraph> para;
                        if (pos < v.size() && parse_paragraph(para, v, pos))
                        {
                            p = pos;
                            o = std::make_shared<_start_scope>(para);
                            return true;
                        }
                        return false;
                    }
                    if (kw == "type")
                    {
                        if (p + 2 < v.size())
                        {
                            try
                            {
                                uint32_t sz = static_cast<uint32_t>(std::stoul(v[p + 2]));
                                o = std::make_shared<_type>(v[p + 1], sz);
                                p += 3;
                                return true;
                            }
                            catch (...) { return false; }
                        }
                        return false;
                    }
                    if (kw == "append_slice")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_append_slice>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "cut_slice")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_cut_slice>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "cut")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_cut>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "cut_len")
                    {
                        if (p + 4 < v.size()) { o = std::make_shared<_cut_len>(v[p + 1], v[p + 2], v[p + 3], v[p + 4]); p += 5; return true; }
                        return false;
                    }
                    if (kw == "clear")
                    {
                        if (p + 1 < v.size()) { o = std::make_shared<_clear>(v[p + 1]); p += 2; return true; }
                        return false;
                    }
                    if (kw == "add")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_add>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "subtract")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_subtract>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "multiply")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_multiply>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "divide")
                    {
                        if (p + 4 < v.size()) { o = std::make_shared<_divide>(v[p + 1], v[p + 2], v[p + 3], v[p + 4]); p += 5; return true; }
                        return false;
                    }
                    if (kw == "extract")
                    {
                        if (p + 4 < v.size()) { o = std::make_shared<_extract>(v[p + 1], v[p + 2], v[p + 3], v[p + 4]); p += 5; return true; }
                        return false;
                    }
                    if (kw == "power")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_power>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "and")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_and>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "or")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_or>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "callee")
                    {
                        if (p + 3 < v.size())
                        {
                            std::string name = v[p + 1];
                            std::string type = v[p + 2];
                            size_t pos = p + 3;
                            std::vector<parameter> params;
                            if (parse_typed_parameters(params, v, pos))
                            {
                                std::shared_ptr<paragraph> body;
                                if (pos < v.size() && parse_paragraph(body, v, pos))
                                {
                                    p = pos;
                                    o = std::make_shared<_callee>(name, type, body, params);
                                    return true;
                                }
                            }
                        }
                        return false;
                    }
                    if (kw == "caller")
                    {
                        if (p + 2 < v.size())
                        {
                            std::string func_name = v[p + 1];
                            // Try 3 args: caller f result (params)
                            if (p + 3 < v.size() && v[p + 3] == "(")
                            {
                                std::string res_name = v[p + 2];
                                size_t pos = p + 3;
                                std::vector<std::string> params;
                                if (parse_parameters(params, v, pos))
                                {
                                    p = pos;
                                    o = std::make_shared<_caller>(func_name, res_name, params);
                                    return true;
                                }
                            }
                            // Try 2 args: caller f (params)
                            if (v[p + 2] == "(")
                            {
                                size_t pos = p + 2;
                                std::vector<std::string> params;
                                if (parse_parameters(params, v, pos))
                                {
                                    p = pos;
                                    o = std::make_shared<_caller>(func_name, params);
                                    return true;
                                }
                            }
                        }
                        return false;
                    }
                    if (kw == "callee_ref")
                    {
                        if (p + 3 < v.size())
                        {
                            std::string name = v[p + 1];
                            std::string type = v[p + 2];
                            size_t pos = p + 3;
                            std::vector<std::string> param_names;
                            if (parse_parameters(param_names, v, pos))
                            {
                                std::vector<parameter_type> params;
                                for (const auto& pn : param_names) params.emplace_back(pn);
                                p = pos;
                                o = std::make_shared<_callee_ref>(name, type, params);
                                return true;
                            }
                        }
                        return false;
                    }
                    if (kw == "caller_ref")
                    {
                        if (p + 2 < v.size())
                        {
                            std::string func_name = v[p + 1];
                            if (p + 3 < v.size() && v[p + 3] == "(")
                            {
                                std::string res_name = v[p + 2];
                                size_t pos = p + 3;
                                std::vector<std::string> params;
                                if (parse_parameters(params, v, pos))
                                {
                                    p = pos;
                                    o = std::make_shared<_caller_ref>(func_name, res_name, params);
                                    return true;
                                }
                            }
                            if (v[p + 2] == "(")
                            {
                                size_t pos = p + 2;
                                std::vector<std::string> params;
                                if (parse_parameters(params, v, pos))
                                {
                                    p = pos;
                                    o = std::make_shared<_caller_ref>(func_name, params);
                                    return true;
                                }
                            }
                        }
                        return false;
                    }
                    if (kw == "address_of")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_address_of>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "less")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_less>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "more")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_more>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "equal")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_equal>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "less_or_equal")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_less_or_equal>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "more_or_equal")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_more_or_equal>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "if")
                    {
                        if (p + 2 < v.size())
                        {
                            std::string cond = v[p + 1];
                            size_t pos = p + 2;
                            std::shared_ptr<paragraph> true_p;
                            if (parse_paragraph(true_p, v, pos))
                            {
                                if (pos < v.size() && v[pos] == "else")
                                {
                                    pos++;
                                    std::shared_ptr<paragraph> false_p;
                                    if (pos < v.size() && parse_paragraph(false_p, v, pos))
                                    {
                                        p = pos;
                                        o = std::make_shared<_if>(cond, true_p, false_p);
                                        return true;
                                    }
                                }
                                else
                                {
                                    p = pos;
                                    o = std::make_shared<_if>(cond, true_p);
                                    return true;
                                }
                            }
                        }
                        return false;
                    }
                    if (kw == "copy")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_copy>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "copy_const")
                    {
                        if (p + 2 < v.size())
                        {
                            primitive::data_block db;
                            if (primitive::parser::parse_data_block(v[p + 2], db))
                            {
                                o = std::make_shared<_copy_const>(v[p + 1], std::move(db));
                                p += 3;
                                return true;
                            }
                        }
                        return false;
                    }
                    if (kw == "define")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_define>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "define_heap")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_define_heap>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "redefine")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_redefine>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "undefine")
                    {
                        if (p + 1 < v.size()) { o = std::make_shared<_undefine>(v[p + 1]); p += 2; return true; }
                        return false;
                    }
                    if (kw == "dealloc_heap")
                    {
                        if (p + 1 < v.size()) { o = std::make_shared<_dealloc_heap>(v[p + 1]); p += 2; return true; }
                        return false;
                    }
                    if (kw == "do_until")
                    {
                        if (p + 2 < v.size())
                        {
                            std::string cond = v[p + 1];
                            size_t pos = p + 2;
                            std::shared_ptr<paragraph> body;
                            if (parse_paragraph(body, v, pos))
                            {
                                p = pos;
                                o = std::make_shared<_do_until>(cond, body);
                                return true;
                            }
                        }
                        return false;
                    }
                    if (kw == "do_while")
                    {
                        if (p + 2 < v.size())
                        {
                            std::string cond = v[p + 1];
                            size_t pos = p + 2;
                            std::shared_ptr<paragraph> body;
                            if (parse_paragraph(body, v, pos))
                            {
                                p = pos;
                                o = std::make_shared<_do_while>(cond, body);
                                return true;
                            }
                        }
                        return false;
                    }
                    if (kw == "interrupt")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_interrupt>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "move")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_move>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "return")
                    {
                        if (p + 2 < v.size())
                        {
                            o = std::make_shared<_return>(v[p + 1], v[p + 2]);
                            p += 3;
                            return true;
                        }
                        return false;
                    }
                    if (kw == "append")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_append>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "not")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_not>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "sizeof")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_sizeof>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "empty")
                    {
                        if (p + 2 < v.size()) { o = std::make_shared<_empty>(v[p + 1], v[p + 2]); p += 3; return true; }
                        return false;
                    }
                    if (kw == "while_then")
                    {
                        if (p + 2 < v.size())
                        {
                            std::string cond = v[p + 1];
                            size_t pos = p + 2;
                            std::shared_ptr<paragraph> body;
                            if (parse_paragraph(body, v, pos))
                            {
                                p = pos;
                                o = std::make_shared<_while_then>(cond, body);
                                return true;
                            }
                        }
                        return false;
                    }
                    if (kw == "stop")
                    {
                        o = std::make_shared<_stop>();
                        p += 1;
                        return true;
                    }
                    if (kw == "float_add")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_add>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_subtract")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_subtract>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_multiply")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_multiply>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_divide")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_divide>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_extract")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_extract>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_power")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_power>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_less")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_less>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_more")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_more>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_equal")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_equal>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_less_or_equal")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_less_or_equal>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "float_more_or_equal")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_float_more_or_equal>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "left_shift")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_left_shift>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }
                    if (kw == "right_shift")
                    {
                        if (p + 3 < v.size()) { o = std::make_shared<_right_shift>(v[p + 1], v[p + 2], v[p + 3]); p += 4; return true; }
                        return false;
                    }

                    errors::unexpected_token(v[p]);
                    return false;
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
