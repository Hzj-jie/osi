#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>
#include "command_type.hpp"
#include "data_ref.hpp"
#include "data_block.hpp"
#include "instruction.hpp"

namespace primitive
{
    class parser
    {
    public:
        static bool parse_data_ref(std::string_view s, data_ref& out)
        {
            while (!s.empty() && std::isspace(s.front())) s.remove_prefix(1);
            while (!s.empty() && std::isspace(s.back())) s.remove_suffix(1);
            if (s.empty()) return false;

            ref_type type = ref_type::abs;
            std::string_view prefix;

            if (s.rfind("habs", 0) == 0) { type = ref_type::habs; prefix = "habs"; }
            else if (s.rfind("hrel", 0) == 0) { type = ref_type::hrel; prefix = "hrel"; }
            else if (s.rfind("abs", 0) == 0) { type = ref_type::abs; prefix = "abs"; }
            else if (s.rfind("rel", 0) == 0) { type = ref_type::rel; prefix = "rel"; }
            else return false;

            s.remove_prefix(prefix.size());
            if (!s.empty() && s.front() == '(' && s.back() == ')')
            {
                s.remove_prefix(1);
                s.remove_suffix(1);
            }

            try
            {
                int64_t offset = std::stoll(std::string(s));
                switch (type)
                {
                    case ref_type::abs: return data_ref::abs(offset, out);
                    case ref_type::rel: return data_ref::rel(offset, out);
                    case ref_type::habs: return data_ref::habs(offset, out);
                    case ref_type::hrel: return data_ref::hrel(offset, out);
                }
            }
            catch (...)
            {
                return false;
            }
            return false;
        }

        static std::string unescape_string(std::string_view sv)
        {
            std::string res;
            res.reserve(sv.size());
            for (size_t i = 0; i < sv.size(); ++i)
            {
                if (sv[i] == '\\' && i + 1 < sv.size())
                {
                    char next = sv[++i];
                    if (next == 'n') res += '\n';
                    else if (next == 'r') res += '\r';
                    else if (next == 't') res += '\t';
                    else if (next == '"') res += '"';
                    else if (next == '\\') res += '\\';
                    else res += next;
                }
                else
                {
                    res += sv[i];
                }
            }
            return res;
        }

        static bool parse_data_block(std::string_view s, data_block& out)
        {
            while (!s.empty() && std::isspace(s.front())) s.remove_prefix(1);
            while (!s.empty() && std::isspace(s.back())) s.remove_suffix(1);
            if (s.empty()) return false;

            char prefix = s.front();
            std::string_view val = s.substr(1);

            try
            {
                if (prefix == 'i')
                {
                    out = data_block::from_int32(std::stoi(std::string(val)));
                    return true;
                }
                else if (prefix == 'l')
                {
                    out = data_block::from_int64(std::stoll(std::string(val)));
                    return true;
                }
                else if (prefix == 'd')
                {
                    out = data_block::from_double(std::stod(std::string(val)));
                    return true;
                }
                else if (prefix == 'b')
                {
                    bool b = (val == "1" || val == "true" || val == "True");
                    out = data_block::from_bool(b);
                    return true;
                }
                else if (prefix == 's' || prefix == 'E')
                {
                    if (val.size() >= 2 && val.front() == '"' && val.back() == '"')
                    {
                        val.remove_prefix(1);
                        val.remove_suffix(1);
                    }
                    out = data_block::from_string(unescape_string(val));
                    return true;
                }
            }
            catch (...)
            {
                return false;
            }
            return false;
        }

        static command_type parse_command_type(std::string_view cmd_str)
        {
            if (cmd_str == "push") return command_type::push;
            if (cmd_str == "pop") return command_type::pop;
            if (cmd_str == "jump") return command_type::jump;
            if (cmd_str == "cpc") return command_type::cpc;
            if (cmd_str == "mov") return command_type::mov;
            if (cmd_str == "cp") return command_type::cp;
            if (cmd_str == "add") return command_type::add;
            if (cmd_str == "sub") return command_type::sub;
            if (cmd_str == "mul") return command_type::mul;
            if (cmd_str == "div") return command_type::div;
            if (cmd_str == "ext") return command_type::ext;
            if (cmd_str == "pow") return command_type::pow;
            if (cmd_str == "jumpif") return command_type::jumpif;
            if (cmd_str == "cpco") return command_type::cpco;
            if (cmd_str == "cpdbz") return command_type::cpdbz;
            if (cmd_str == "cpin") return command_type::cpin;
            if (cmd_str == "stop") return command_type::stop;
            if (cmd_str == "equal") return command_type::equal;
            if (cmd_str == "less") return command_type::less;
            if (cmd_str == "app") return command_type::app;
            if (cmd_str == "sapp") return command_type::sapp;
            if (cmd_str == "cut") return command_type::cut;
            if (cmd_str == "cutl") return command_type::cutl;
            if (cmd_str == "int") return command_type::interrupt_cmd;
            if (cmd_str == "clr") return command_type::clr;
            if (cmd_str == "scut") return command_type::scut;
            if (cmd_str == "sizeof") return command_type::sizeof_cmd;
            if (cmd_str == "empty") return command_type::empty;
            if (cmd_str == "and") return command_type::cmd_and;
            if (cmd_str == "or") return command_type::cmd_or;
            if (cmd_str == "not") return command_type::cmd_not;
            if (cmd_str == "stst") return command_type::stst;
            if (cmd_str == "rest") return command_type::rest;
            if (cmd_str == "fadd") return command_type::fadd;
            if (cmd_str == "fsub") return command_type::fsub;
            if (cmd_str == "fmul") return command_type::fmul;
            if (cmd_str == "fdiv") return command_type::fdiv;
            if (cmd_str == "fext") return command_type::fext;
            if (cmd_str == "fpow") return command_type::fpow;
            if (cmd_str == "fequal") return command_type::fequal;
            if (cmd_str == "fless") return command_type::fless;
            if (cmd_str == "lfs") return command_type::lfs;
            if (cmd_str == "rfs") return command_type::rfs;
            if (cmd_str == "alloc") return command_type::alloc;
            if (cmd_str == "dealloc") return command_type::dealloc;
            if (cmd_str == "jmpr") return command_type::jmpr;
            return command_type::unknown;
        }

        static std::vector<std::string> tokenize(std::string_view line)
        {
            std::vector<std::string> tokens;
            size_t i = 0;
            while (i < line.size())
            {
                while (i < line.size() && std::isspace(line[i])) ++i;
                if (i >= line.size()) break;

                std::string token;
                size_t start = i;
                while (i < line.size() && !std::isspace(line[i]))
                {
                    if (line[i] == '"')
                    {
                        token += line.substr(start, i - start);
                        token += line[i++];
                        while (i < line.size() && line[i] != '"')
                        {
                            if (line[i] == '\\' && i + 1 < line.size())
                            {
                                token += line[i++];
                            }
                            token += line[i++];
                        }
                        if (i < line.size()) token += line[i++];
                        start = i;
                        break;
                    }
                    else
                    {
                        ++i;
                    }
                }
                if (start < i) token += line.substr(start, i - start);
                if (!token.empty()) tokens.push_back(token);
            }
            return tokens;
        }

        static bool parse_instruction(std::string_view line, instruction& out)
        {
            // Trim comment (#...)
            size_t hash_pos = line.find('#');
            if (hash_pos != std::string_view::npos)
                line = line.substr(0, hash_pos);

            std::vector<std::string> tokens = tokenize(line);
            if (tokens.empty()) return false;

            command_type cmd = parse_command_type(tokens[0]);
            if (cmd == command_type::unknown) return false;

            out.cmd = cmd;
            out.operands.clear();
            out.constant_val.bytes.clear();

            if (cmd == command_type::cpc)
            {
                if (tokens.size() < 3) return false;
                data_ref ref;
                if (!parse_data_ref(tokens[1], ref)) return false;
                out.operands.push_back(ref);

                data_block db;
                if (!parse_data_block(tokens[2], db)) return false;
                out.constant_val = std::move(db);
                return true;
            }
            else
            {
                for (size_t i = 1; i < tokens.size(); ++i)
                {
                    data_ref ref;
                    if (!parse_data_ref(tokens[i], ref)) return false;
                    out.operands.push_back(ref);
                }
                return true;
            }
        }

        static bool parse_program(const std::string& src, std::vector<instruction>& out)
        {
            out.clear();
            std::istringstream iss(src);
            std::string line;
            while (std::getline(iss, line))
            {
                instruction inst;
                if (parse_instruction(line, inst))
                {
                    out.push_back(std::move(inst));
                }
            }
            return !out.empty();
        }
    };
}
