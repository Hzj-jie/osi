#pragma once
#include <vector>
#include "instruction.hpp"
#include "registers.hpp"
#include "memory_space.hpp"
#include "executor.hpp"

namespace primitive
{
    class simulator
    {
    private:
        std::vector<instruction> instructions_;
        registers reg_;
        memory_space mem_;

    public:
        simulator() = default;

        void load_instructions(std::vector<instruction> insts)
        {
            instructions_ = std::move(insts);
            instructions_.push_back(instruction(command_type::stop));
            reset();
        }

        void reset()
        {
            reg_.reset_all();
            mem_.clear();
        }

        registers& reg() { return reg_; }
        const registers& reg() const { return reg_; }

        memory_space& mem() { return mem_; }
        const memory_space& mem() const { return mem_; }

        const std::vector<instruction>& instructions() const { return instructions_; }

        bool step()
        {
            if (reg_.halt || reg_.stop) return false;
            if (reg_.ip >= instructions_.size())
            {
                reg_.halt = true;
                return false;
            }

            const auto& inst = instructions_[static_cast<size_t>(reg_.ip)];
            if (!executor::execute_instruction(inst, reg_, mem_))
            {
                reg_.halt = true;
                return false;
            }

            if (!reg_.do_not_advance_ip)
            {
                reg_.ip++;
            }
            return true;
        }

        bool run(size_t max_steps = 1000000)
        {
            size_t steps = 0;
            while (!reg_.halt && !reg_.stop && steps < max_steps)
            {
                if (!step()) break;
                steps++;
            }
            return reg_.stop;
        }
    };
}
