#pragma once
#include <vector>
#include <cmath>
#include "command_type.hpp"
#include "instruction.hpp"
#include "registers.hpp"
#include "memory_space.hpp"
#include "../app_info/assert.hpp"

namespace primitive
{
    class executor
    {
    public:
        static bool execute_instruction(const instruction& inst, registers& reg, memory_space& mem)
        {
            reg.reset_instruction_flags();

            switch (inst.cmd)
            {
                case command_type::push:
                {
                    mem.push_stack(data_block());
                    break;
                }
                case command_type::pop:
                {
                    data_block dummy;
                    if (!mem.pop_stack(dummy)) return false;
                    break;
                }
                case command_type::cpc:
                {
                    if (inst.operands.empty()) return false;
                    data_block* target = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], target) || target == nullptr) return false;
                    *target = inst.constant_val;
                    break;
                }
                case command_type::cp:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *src = nullptr, *dst = nullptr;
                    if (!mem.resolve_ref(inst.operands[1], src) || !mem.resolve_ref(inst.operands[0], dst)) return false;
                    if (src == nullptr || dst == nullptr) return false;
                    *dst = *src;
                    break;
                }
                case command_type::mov:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *src = nullptr, *dst = nullptr;
                    if (!mem.resolve_ref(inst.operands[1], src) || !mem.resolve_ref(inst.operands[0], dst)) return false;
                    if (src == nullptr || dst == nullptr) return false;
                    *dst = std::move(*src);
                    src->bytes.clear();
                    break;
                }
                case command_type::clr:
                {
                    if (inst.operands.empty()) return false;
                    data_block* target = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], target) || target == nullptr) return false;
                    target->bytes.clear();
                    break;
                }
                case command_type::jump:
                {
                    if (inst.operands.empty()) return false;
                    const auto& ref = inst.operands[0];
                    if (ref.type == ref_type::rel)
                        reg.ip = static_cast<uint64_t>(static_cast<int64_t>(reg.ip) + ref.offset);
                    else
                        reg.ip = static_cast<uint64_t>(ref.offset);
                    reg.do_not_advance_ip = true;
                    break;
                }
                case command_type::jmpr:
                {
                    if (inst.operands.empty()) return false;
                    data_block* target = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], target) || target == nullptr) return false;
                    reg.ip = static_cast<uint64_t>(target->as_int64());
                    reg.do_not_advance_ip = true;
                    break;
                }
                case command_type::jumpif:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block* cond = nullptr;
                    if (!mem.resolve_ref(inst.operands[1], cond) || cond == nullptr) return false;
                    if (cond->as_bool())
                    {
                        const auto& ref = inst.operands[0];
                        if (ref.type == ref_type::rel)
                            reg.ip = static_cast<uint64_t>(static_cast<int64_t>(reg.ip) + ref.offset);
                        else
                            reg.ip = static_cast<uint64_t>(ref.offset);
                        reg.do_not_advance_ip = true;
                    }
                    break;
                }
                case command_type::add:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() + b2->as_int64());
                    break;
                }
                case command_type::sub:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() - b2->as_int64());
                    break;
                }
                case command_type::mul:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() * b2->as_int64());
                    break;
                }
                case command_type::div:
                {
                    if (inst.operands.size() < 4) return false;
                    data_block *quot = nullptr, *rem = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], quot) ||
                        !mem.resolve_ref(inst.operands[1], rem) ||
                        !mem.resolve_ref(inst.operands[2], b1) ||
                        !mem.resolve_ref(inst.operands[3], b2)) return false;
                    if (!quot || !rem || !b1 || !b2) return false;
                    int64_t v2 = b2->as_int64();
                    if (v2 == 0)
                    {
                        reg.divided_by_zero = true;
                    }
                    else
                    {
                        int64_t v1 = b1->as_int64();
                        *quot = data_block::from_int64(v1 / v2);
                        *rem = data_block::from_int64(v1 % v2);
                    }
                    break;
                }
                case command_type::ext:
                {
                    if (inst.operands.size() < 4) return false;
                    data_block *quot = nullptr, *rem = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], quot) ||
                        !mem.resolve_ref(inst.operands[1], rem) ||
                        !mem.resolve_ref(inst.operands[2], b1) ||
                        !mem.resolve_ref(inst.operands[3], b2)) return false;
                    if (!quot || !rem || !b1 || !b2) return false;
                    int64_t v2 = b2->as_int64();
                    if (v2 == 0)
                    {
                        reg.divided_by_zero = true;
                    }
                    else
                    {
                        int64_t v1 = b1->as_int64();
                        *quot = data_block::from_int64(v1 / v2);
                        *rem = data_block::from_int64(v1 % v2);
                    }
                    break;
                }
                case command_type::pow:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    int64_t base = b1->as_int64();
                    int64_t exp = b2->as_int64();
                    *res = data_block::from_int64(static_cast<int64_t>(std::pow(base, exp)));
                    break;
                }
                case command_type::equal:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_bool(b1->as_int64() == b2->as_int64());
                    break;
                }
                case command_type::less:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_bool(b1->as_int64() < b2->as_int64());
                    break;
                }
                case command_type::fadd:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_double(b1->as_double() + b2->as_double());
                    break;
                }
                case command_type::fsub:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_double(b1->as_double() - b2->as_double());
                    break;
                }
                case command_type::fmul:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_double(b1->as_double() * b2->as_double());
                    break;
                }
                case command_type::fdiv:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    double v2 = b2->as_double();
                    if (v2 == 0.0)
                    {
                        reg.divided_by_zero = true;
                    }
                    else
                    {
                        *res = data_block::from_double(b1->as_double() / v2);
                    }
                    break;
                }
                case command_type::fequal:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_bool(b1->as_double() == b2->as_double());
                    break;
                }
                case command_type::fpow:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_double(std::pow(b1->as_double(), b2->as_double()));
                    break;
                }
                case command_type::fext:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    double v2 = b2->as_double();
                    if (v2 == 0.0)
                    {
                        reg.divided_by_zero = true;
                    }
                    else
                    {
                        *res = data_block::from_double(std::fmod(b1->as_double(), v2));
                    }
                    break;
                }
                case command_type::sapp:
                case command_type::app:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *dst = nullptr, *src = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) ||
                        !mem.resolve_ref(inst.operands[1], src)) return false;
                    if (!dst || !src) return false;
                    dst->bytes.insert(dst->bytes.end(), src->bytes.begin(), src->bytes.end());
                    break;
                }
                case command_type::scut:
                case command_type::cut:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *dst = nullptr, *src = nullptr, *offset_block = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) ||
                        !mem.resolve_ref(inst.operands[1], src) ||
                        !mem.resolve_ref(inst.operands[2], offset_block)) return false;
                    if (!dst || !src || !offset_block) return false;
                    size_t offset = static_cast<size_t>(offset_block->as_int64());
                    if (offset >= src->bytes.size())
                    {
                        dst->bytes.clear();
                    }
                    else
                    {
                        dst->bytes.assign(src->bytes.begin() + offset, src->bytes.end());
                    }
                    break;
                }
                case command_type::scutl:
                case command_type::cutl:
                {
                    if (inst.operands.size() < 4) return false;
                    data_block *dst = nullptr, *src = nullptr, *offset_block = nullptr, *len_block = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) ||
                        !mem.resolve_ref(inst.operands[1], src) ||
                        !mem.resolve_ref(inst.operands[2], offset_block) ||
                        !mem.resolve_ref(inst.operands[3], len_block)) return false;
                    if (!dst || !src || !offset_block || !len_block) return false;
                    size_t offset = static_cast<size_t>(offset_block->as_int64());
                    size_t len = static_cast<size_t>(len_block->as_int64());
                    if (offset >= src->bytes.size() || len == 0)
                    {
                        dst->bytes.clear();
                    }
                    else
                    {
                        size_t available = src->bytes.size() - offset;
                        size_t count = std::min(available, len);
                        dst->bytes.assign(src->bytes.begin() + offset, src->bytes.begin() + offset + count);
                    }
                    break;
                }
                case command_type::cmd_and:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() & b2->as_int64());
                    break;
                }
                case command_type::cmd_or:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() | b2->as_int64());
                    break;
                }
                case command_type::cmd_not:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *res = nullptr, *b1 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1)) return false;
                    if (!res || !b1) return false;
                    *res = data_block::from_int64(~b1->as_int64());
                    break;
                }
                case command_type::interrupt_cmd:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *id_block = nullptr, *param_block = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], id_block) ||
                        !mem.resolve_ref(inst.operands[2], param_block)) return false;
                    if (!res || !id_block || !param_block) return false;
                    uint32_t int_id = static_cast<uint32_t>(id_block->as_int64());
                    res->bytes = mem.intr().invoke(int_id, param_block->bytes);
                    break;
                }
                case command_type::lfs:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() << b2->as_int64());
                    break;
                }
                case command_type::rfs:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    *res = data_block::from_int64(b1->as_int64() >> b2->as_int64());
                    break;
                }
                case command_type::stst:
                {
                    mem.push_call_state(reg.ip + 2, mem.stack_size());
                    break;
                }
                case command_type::rest:
                {
                    call_state cs;
                    if (!mem.pop_call_state(cs)) return false;
                    while (mem.stack_size() > cs.stack_size)
                    {
                        data_block dummy;
                        mem.pop_stack(dummy);
                    }
                    reg.ip = cs.return_ip;
                    reg.do_not_advance_ip = true;
                    break;
                }
                case command_type::alloc:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *target = nullptr, *size_block = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], target) ||
                        !mem.resolve_ref(inst.operands[1], size_block)) return false;
                    if (!target || !size_block) return false;
                    uint64_t heap_id = mem.alloc_heap(static_cast<size_t>(size_block->as_int64()));
                    *target = data_block::from_int64(static_cast<int64_t>(heap_id));
                    break;
                }
                case command_type::dealloc:
                {
                    if (inst.operands.empty()) return false;
                    data_block* target = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], target) || target == nullptr) return false;
                    uint64_t heap_id = static_cast<uint64_t>(target->as_int64());
                    if (!mem.free_heap(heap_id)) return false;
                    break;
                }
                case command_type::sizeof_cmd:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *res = nullptr, *src = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], src)) return false;
                    if (!res || !src) return false;
                    *res = data_block::from_int64(static_cast<int64_t>(src->bytes.size()));
                    break;
                }
                case command_type::empty:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *res = nullptr, *src = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], src)) return false;
                    if (!res || !src) return false;
                    *res = data_block::from_bool(src->bytes.empty());
                    break;
                }
                case command_type::cpco:
                {
                    if (inst.operands.empty()) return false;
                    data_block* dst = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) || !dst) return false;
                    *dst = data_block::from_bool(reg.carry_over);
                    break;
                }
                case command_type::cpdbz:
                {
                    if (inst.operands.empty()) return false;
                    data_block* dst = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) || !dst) return false;
                    *dst = data_block::from_bool(reg.divided_by_zero);
                    break;
                }
                case command_type::cpin:
                {
                    if (inst.operands.empty()) return false;
                    data_block* dst = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) || !dst) return false;
                    *dst = data_block::from_bool(reg.imaginary_number);
                    break;
                }
                case command_type::stop:
                {
                    reg.stop = true;
                    reg.halt = true;
                    break;
                }
                default:
                    return false;
            }

            return true;
        }
    };
}
