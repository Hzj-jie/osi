#pragma once
#include <string>
#include "instruction_gen.hpp"
#include "variable.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class copy_move : public instruction_gen
            {
            protected:
                std::string target;
                std::string source;
                std::string cmd;

                copy_move(std::string target, std::string source, std::string cmd)
                    : target(std::move(target)), source(std::move(source)), cmd(std::move(cmd))
                {
                    assert(!this->target.empty());
                    assert(!this->source.empty());
                    assert(!this->cmd.empty());
                }

            public:
                static bool export_instruction(const std::string& cmd, const variable& target, const variable& source, std::vector<std::string>& o)
                {
                    if (target.is_assignable_from(source))
                    {
                        o.push_back(strcat(cmd, " ", target.ToString(), " ", source.ToString()));
                        return true;
                    }
                    return false;
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable t, s;
                    return variable::of(target, o, t) &&
                           variable::of(source, o, s) &&
                           export_instruction(cmd, t, s, o);
                }
            };

            class _copy : public copy_move
            {
            public:
                _copy(std::string target, std::string source)
                    : copy_move(std::move(target), std::move(source), "cp") {}

                static bool export_instruction(const variable& target, const variable& source, std::vector<std::string>& o)
                {
                    return copy_move::export_instruction("cp", target, source, o);
                }
            };

            class _move : public copy_move
            {
            public:
                _move(std::string target, std::string source)
                    : copy_move(std::move(target), std::move(source), "mov") {}

                static bool export_instruction(const variable& target, const variable& source, std::vector<std::string>& o)
                {
                    return copy_move::export_instruction("mov", target, source, o);
                }
            };
        }
    }
}
