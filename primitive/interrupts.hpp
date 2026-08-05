#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <chrono>
#include <cstdint>
#include "../app_info/assert.hpp"
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
            {
                assert(false, "Unsupported interrupt ID: ", id);
                return {};
            }
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
            // 2: stdin
            register_handler(2, "stdin", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                std::vector<uint8_t> out;
                char c;
                while (std::cin.get(c))
                {
                    out.push_back(static_cast<uint8_t>(c));
                }
                return out;
            });
            // 3: current_ms
            register_handler(3, "current_ms", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                data_block db = data_block::from_int64(static_cast<int64_t>(now));
                return db.bytes;
            });
            // 4: load_method (unsupported in C++)
            register_handler(4, "load_method", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                assert(false, "load_method interrupt is unsupported in C++ implementation");
                return {};
            });
            // 5: execute_loaded_method (unsupported in C++)
            register_handler(5, "execute_loaded_method", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                assert(false, "execute_loaded_method interrupt is unsupported in C++ implementation");
                return {};
            });
            // 6: getchar
            register_handler(6, "getchar", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                int c = std::cin.get();
                int32_t val = (c == EOF) ? -1 : static_cast<int32_t>(c);
                return data_block::from_int32(val).bytes;
            });
            // 7: putchar
            register_handler(7, "putchar", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                if (in.size() >= sizeof(int32_t))
                {
                    int32_t val = 0;
                    std::memcpy(&val, in.data(), sizeof(int32_t));
                    std::cout.put(static_cast<char>(val));
                    std::cout.flush();
                }
                return {};
            });
        }
    };
}
