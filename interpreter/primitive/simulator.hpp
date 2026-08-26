#pragma once
#include <vector>
#include "instruction.hpp"
#include "registers.hpp"
#include "memory_space.hpp"
#include "executor.hpp"

namespace primitive {
    class simulator {
    private:
        std::vector<instruction> instructions_;
        registers reg_;
        memory_space mem_;

    public:
        simulator() = default;

        explicit simulator(interrupts intr) {
            mem_.intr() = std::move(intr);
        }

        void load_instructions(std::vector<instruction> insts) {
            instructions_ = std::move(insts);
            instructions_.push_back(instruction(command_type::stop));
            reset();
        }

        void reset() {
            reg_.reset_all();
            mem_.clear();
        }

        registers& reg() { return reg_; }
        const registers& reg() const { return reg_; }

        memory_space& mem() { return mem_; }
        const memory_space& mem() const { return mem_; }

        const std::vector<instruction>& instructions() const { return instructions_; }

        size_t stack_size() const { return mem_.stack_size(); }
        bool halt() const { return reg_.halt; }
        std::string halt_error() const { return "executor halted at instruction " + std::to_string(reg_.ip); }

        uint32_t access_as_uint32(const data_ref& ref) const {
            data_block* b = nullptr;
            if (const_cast<memory_space&>(mem_).resolve_ref(ref, b) && b) {
                return static_cast<uint32_t>(b->as_int64());
            }
            return 0;
        }

        bool access_as_bool(const data_ref& ref) const {
            data_block* b = nullptr;
            if (const_cast<memory_space&>(mem_).resolve_ref(ref, b) && b) {
                return b->as_bool();
            }
            return false;
        }

        std::vector<uint8_t> access(const data_ref& ref) const {
            data_block* b = nullptr;
            if (const_cast<memory_space&>(mem_).resolve_ref(ref, b) && b) {
                return b->bytes;
            }
            return {};
        }

        void execute() {
            run();
        }

        bool step() {
            if (reg_.halt || reg_.stop) return false;
            if (reg_.ip >= instructions_.size()) {
                reg_.halt = true;
                return false;
            }

            const auto& inst = instructions_[static_cast<size_t>(reg_.ip)];
            if (!executor::execute_instruction(inst, reg_, mem_)) {
                reg_.halt = true;
                return false;
            }

            if (!reg_.do_not_advance_ip) {
                reg_.ip++;
            }
            return true;
        }

        bool run(size_t max_steps = 1000000) {
            size_t steps = 0;
            while (!reg_.halt && !reg_.stop && steps < max_steps) {
                if (!step()) break;
                steps++;
            }
            return reg_.stop;
        }
    };
}
