#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>
#include <cstdint>
#include "data_block.hpp"

namespace primitive
{
    using interrupt_handler = std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>;

    class interrupts
    {
    private:
        std::unordered_map<uint32_t, interrupt_handler> handlers_;
        std::unordered_map<std::string, uint32_t> name_to_id_;

    public:
        interrupts()
        {
            register_default_handlers();
        }

        void register_handler(uint32_t id, std::string name, interrupt_handler handler)
        {
            handlers_[id] = std::move(handler);
            if (!name.empty())
                name_to_id_[name] = id;
        }

        std::vector<uint8_t> invoke(uint32_t id, const std::vector<uint8_t>& param) const
        {
            auto it = handlers_.find(id);
            if (it == handlers_.end())
                return {};
            return it->second(param);
        }

        bool get_id(const std::string& name, uint32_t& out_id) const
        {
            auto it = name_to_id_.find(name);
            if (it == name_to_id_.end()) return false;
            out_id = it->second;
            return true;
        }

    private:
        void register_default_handlers()
        {
            // 0: stdout
            register_handler(0, "stdout", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                std::cout.write(reinterpret_cast<const char*>(in.data()), in.size());
                std::cout.flush();
                return {};
            });
            // 1: stderr
            register_handler(1, "stderr", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                std::cerr.write(reinterpret_cast<const char*>(in.data()), in.size());
                std::cerr.flush();
                return {};
            });
            // 2: current_ms
            register_handler(2, "current_ms", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                data_block db = data_block::from_int64(static_cast<int64_t>(now));
                return db.bytes;
            });
        }
    };
}
