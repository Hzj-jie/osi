#pragma once
#include <vector>
#include <cstdint>
#include <cstring>
#include "instruction.hpp"

namespace primitive
{
    class bytecode
    {
    public:
        static std::vector<uint8_t> export_bytes(const std::vector<instruction>& insts)
        {
            std::vector<uint8_t> out;

            auto append_u8 = [&](uint8_t v) { out.push_back(v); };
            auto append_u32 = [&](uint32_t v) {
                uint8_t buf[4];
                std::memcpy(buf, &v, 4);
                out.insert(out.end(), buf, buf + 4);
            };
            auto append_i64 = [&](int64_t v) {
                uint8_t buf[8];
                std::memcpy(buf, &v, 8);
                out.insert(out.end(), buf, buf + 8);
            };

            append_u32(static_cast<uint32_t>(insts.size()));

            for (const auto& inst : insts)
            {
                append_u8(static_cast<uint8_t>(inst.cmd));
                append_u8(static_cast<uint8_t>(inst.operands.size()));
                for (const auto& op : inst.operands)
                {
                    append_i64(op.encode());
                }

                if (inst.cmd == command_type::cpc)
                {
                    append_u32(static_cast<uint32_t>(inst.constant_val.bytes.size()));
                    out.insert(out.end(), inst.constant_val.bytes.begin(), inst.constant_val.bytes.end());
                }
            }

            return out;
        }

        static bool import_bytes(const uint8_t* data, size_t size, std::vector<instruction>& out)
        {
            out.clear();
            if (size < 4) return false;

            size_t pos = 0;
            auto read_u8 = [&](uint8_t& v) -> bool {
                if (pos + 1 > size) return false;
                v = data[pos++];
                return true;
            };
            auto read_u32 = [&](uint32_t& v) -> bool {
                if (pos + 4 > size) return false;
                std::memcpy(&v, data + pos, 4);
                pos += 4;
                return true;
            };
            auto read_i64 = [&](int64_t& v) -> bool {
                if (pos + 8 > size) return false;
                std::memcpy(&v, data + pos, 8);
                pos += 8;
                return true;
            };

            uint32_t count = 0;
            if (!read_u32(count)) return false;

            out.reserve(count);
            for (uint32_t i = 0; i < count; ++i)
            {
                uint8_t cmd_raw = 0;
                uint8_t op_count = 0;
                if (!read_u8(cmd_raw) || !read_u8(op_count)) return false;

                instruction inst;
                inst.cmd = static_cast<command_type>(cmd_raw);

                for (uint8_t j = 0; j < op_count; ++j)
                {
                    int64_t raw_ref = 0;
                    if (!read_i64(raw_ref)) return false;

                    data_ref ref;
                    if (!data_ref::parse(raw_ref, ref)) return false;
                    inst.operands.push_back(ref);
                }

                if (inst.cmd == command_type::cpc)
                {
                    uint32_t val_size = 0;
                    if (!read_u32(val_size)) return false;
                    if (pos + val_size > size) return false;

                    inst.constant_val.bytes.assign(data + pos, data + pos + val_size);
                    pos += val_size;
                }

                out.push_back(std::move(inst));
            }

            return true;
        }

        static bool import_bytes(const std::vector<uint8_t>& data, std::vector<instruction>& out)
        {
            return import_bytes(data.data(), data.size(), out);
        }
    };
}
