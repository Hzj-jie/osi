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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    v1.add(v2);
                    res->bytes = v1.as_bytes();
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    if (v1 < v2)
                    {
                        reg.carry_over = true;
                        res->bytes = {0};
                    }
                    else
                    {
                        reg.carry_over = false;
                        v1.sub(v2);
                        res->bytes = v1.as_bytes();
                    }
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    v1.multiply(v2);
                    res->bytes = v1.as_bytes();
                    break;
                }
                case command_type::div:
                case command_type::ext:
                {
                    if (inst.operands.size() < 4) return false;
                    data_block *quot = nullptr, *rem = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], quot) ||
                        !mem.resolve_ref(inst.operands[1], rem) ||
                        !mem.resolve_ref(inst.operands[2], b1) ||
                        !mem.resolve_ref(inst.operands[3], b2)) return false;
                    if (!quot || !rem || !b1 || !b2) return false;
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    if (v2.is_zero())
                    {
                        reg.divided_by_zero = true;
                    }
                    else
                    {
                        reg.divided_by_zero = false;
                        osi::math::big_uint remainder;
                        osi::math::big_uint quotient = v1.divide(v2, remainder);
                        quot->bytes = quotient.as_bytes();
                        rem->bytes = remainder.as_bytes();
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    v1.power(v2.as_uint32());
                    res->bytes = v1.as_bytes();
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    *res = data_block::from_bool(v1 == v2);
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    *res = data_block::from_bool(v1 < v2);
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
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_udec v2(b2->bytes);
                    v1.add(v2);
                    res->bytes = v1.as_bytes();
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
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_udec v2(b2->bytes);
                    if (v1 < v2)
                    {
                        reg.imaginary_number = true;
                        res->bytes = {0};
                    }
                    else
                    {
                        reg.imaginary_number = false;
                        v1.sub(v2);
                        res->bytes = v1.as_bytes();
                    }
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
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_udec v2(b2->bytes);
                    v1.multiply(v2);
                    res->bytes = v1.as_bytes();
                    break;
                }
                case command_type::fdiv:
                case command_type::fext:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_udec v2(b2->bytes);
                    if (v2.is_zero())
                    {
                        reg.divided_by_zero = true;
                    }
                    else
                    {
                        reg.divided_by_zero = false;
                        v1.divide(v2);
                        res->bytes = v1.as_bytes();
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
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_udec v2(b2->bytes);
                    *res = data_block::from_bool(v1 == v2);
                    break;
                }
                case command_type::fless:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *res = nullptr, *b1 = nullptr, *b2 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1) ||
                        !mem.resolve_ref(inst.operands[2], b2)) return false;
                    if (!res || !b1 || !b2) return false;
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_udec v2(b2->bytes);
                    *res = data_block::from_bool(v1 < v2);
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
                    osi::math::big_udec v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    v1.power(v2.as_uint32());
                    res->bytes = v1.as_bytes();
                    break;
                }
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
                case command_type::sapp:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *dst = nullptr, *src = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) ||
                        !mem.resolve_ref(inst.operands[1], src)) return false;
                    if (!dst || !src) return false;
                    constexpr uint32_t chunk_checksum_mask = 0x00480048;
                    uint32_t len = static_cast<uint32_t>(src->bytes.size());
                    uint32_t chk = len ^ chunk_checksum_mask;
                    dst->bytes.push_back(static_cast<uint8_t>(len & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>((len >> 16) & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>((len >> 24) & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>(chk & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>((chk >> 8) & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>((chk >> 16) & 0xFF));
                    dst->bytes.push_back(static_cast<uint8_t>((chk >> 24) & 0xFF));
                    dst->bytes.insert(dst->bytes.end(), src->bytes.begin(), src->bytes.end());
                    break;
                }
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
                case command_type::scut:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *dst = nullptr, *src = nullptr, *offset_block = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) ||
                        !mem.resolve_ref(inst.operands[1], src) ||
                        !mem.resolve_ref(inst.operands[2], offset_block)) return false;
                    if (!dst || !src || !offset_block) return false;
                    size_t target_idx = static_cast<size_t>(offset_block->as_int64());
                    constexpr uint32_t chunk_checksum_mask = 0x00480048;
                    size_t offset = 0;
                    size_t cur_idx = 0;
                    bool found = false;
                    while (offset + 8 <= src->bytes.size())
                    {
                        uint32_t len = static_cast<uint32_t>(src->bytes[offset]) |
                                       (static_cast<uint32_t>(src->bytes[offset+1]) << 8) |
                                       (static_cast<uint32_t>(src->bytes[offset+2]) << 16) |
                                       (static_cast<uint32_t>(src->bytes[offset+3]) << 24);
                        uint32_t chk = static_cast<uint32_t>(src->bytes[offset+4]) |
                                       (static_cast<uint32_t>(src->bytes[offset+5]) << 8) |
                                       (static_cast<uint32_t>(src->bytes[offset+6]) << 16) |
                                       (static_cast<uint32_t>(src->bytes[offset+7]) << 24);
                        if (chk != (len ^ chunk_checksum_mask)) break;
                        offset += 8;
                        if (offset + len > src->bytes.size()) break;
                        if (cur_idx == target_idx)
                        {
                            dst->bytes.assign(src->bytes.begin() + offset, src->bytes.begin() + offset + len);
                            found = true;
                            break;
                        }
                        offset += len;
                        cur_idx++;
                    }
                    if (!found)
                    {
                        dst->bytes.clear();
                    }
                    break;
                }
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
                case command_type::scutl:
                {
                    if (inst.operands.size() < 4) return false;
                    data_block *dst = nullptr, *src = nullptr, *offset_block = nullptr, *len_block = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], dst) ||
                        !mem.resolve_ref(inst.operands[1], src) ||
                        !mem.resolve_ref(inst.operands[2], offset_block) ||
                        !mem.resolve_ref(inst.operands[3], len_block)) return false;
                    if (!dst || !src || !offset_block || !len_block) return false;
                    size_t target_offset = static_cast<size_t>(offset_block->as_int64());
                    size_t target_count = static_cast<size_t>(len_block->as_int64());
                    constexpr uint32_t chunk_checksum_mask = 0x00480048;
                    size_t offset = 0;
                    size_t cur_idx = 0;
                    dst->bytes.clear();
                    while (offset + 8 <= src->bytes.size() && target_count > 0)
                    {
                        uint32_t len = static_cast<uint32_t>(src->bytes[offset]) |
                                       (static_cast<uint32_t>(src->bytes[offset+1]) << 8) |
                                       (static_cast<uint32_t>(src->bytes[offset+2]) << 16) |
                                       (static_cast<uint32_t>(src->bytes[offset+3]) << 24);
                        uint32_t chk = static_cast<uint32_t>(src->bytes[offset+4]) |
                                       (static_cast<uint32_t>(src->bytes[offset+5]) << 8) |
                                       (static_cast<uint32_t>(src->bytes[offset+6]) << 16) |
                                       (static_cast<uint32_t>(src->bytes[offset+7]) << 24);
                        if (chk != (len ^ chunk_checksum_mask)) break;
                        size_t chunk_total = 8 + len;
                        if (offset + chunk_total > src->bytes.size()) break;
                        if (cur_idx >= target_offset && cur_idx < target_offset + target_count)
                        {
                            dst->bytes.insert(dst->bytes.end(), src->bytes.begin() + offset, src->bytes.begin() + offset + chunk_total);
                        }
                        offset += chunk_total;
                        cur_idx++;
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    osi::math::big_uint r = v1 & v2;
                    res->bytes = r.as_bytes();
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    osi::math::big_uint r = v1 | v2;
                    res->bytes = r.as_bytes();
                    break;
                }
                case command_type::cmd_not:
                {
                    if (inst.operands.size() < 2) return false;
                    data_block *res = nullptr, *b1 = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], res) ||
                        !mem.resolve_ref(inst.operands[1], b1)) return false;
                    if (!res || !b1) return false;
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint r = ~v1;
                    res->bytes = r.as_bytes();
                    break;
                }
                case command_type::interrupt_cmd:
                {
                    if (inst.operands.size() < 3) return false;
                    data_block *id_block = nullptr, *param_block = nullptr, *res = nullptr;
                    if (!mem.resolve_ref(inst.operands[0], id_block) ||
                        !mem.resolve_ref(inst.operands[1], param_block) ||
                        !mem.resolve_ref(inst.operands[2], res)) return false;
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    osi::math::big_uint r = v1 << static_cast<size_t>(v2.as_uint64());
                    res->bytes = r.as_bytes();
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
                    osi::math::big_uint v1(b1->bytes);
                    osi::math::big_uint v2(b2->bytes);
                    osi::math::big_uint r = v1 >> static_cast<size_t>(v2.as_uint64());
                    res->bytes = r.as_bytes();
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
                    *res = data_block::from_int32(static_cast<int32_t>(src->bytes.size()));
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
