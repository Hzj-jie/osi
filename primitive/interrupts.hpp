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

namespace primitive {
    using method_handler = std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)>;
    using interrupt_handler = method_handler;

    class loaded_methods {
    private:
        static std::unordered_map<std::string, method_handler>& global_registry() {
            static std::unordered_map<std::string, method_handler> reg;
            return reg;
        }

        method_handler last_loaded_;

    public:
        loaded_methods() {
            register_default_methods();
        }

        static void register_method(const std::string& name, method_handler handler) {
            global_registry()[name] = std::move(handler);
        }

        bool load(const std::vector<uint8_t>& name_bytes) {
            std::string name(reinterpret_cast<const char*>(name_bytes.data()), name_bytes.size());
            auto& reg = global_registry();
            auto it = reg.find(name);
            if (it == reg.end()) {
                // Try searching after colon or comma if fully qualified
                size_t idx = name.rfind(':');
                if (idx != std::string::npos) {
                    std::string short_name = name.substr(idx + 1);
                    it = reg.find(short_name);
                }
            }
            if (it == reg.end()) {
                last_loaded_ = nullptr;
                return false;
            }
            last_loaded_ = it->second;
            return true;
        }

        std::vector<uint8_t> execute(const std::vector<uint8_t>& param, bool& ok) const {
            if (!last_loaded_) {
                ok = false;
                return {};
            }
            ok = true;
            return last_loaded_(param);
        }

    private:
        static std::string big_bytes_to_decimal_string(const std::vector<uint8_t>& bytes) {
            if (bytes.empty()) return "0";
            std::vector<int> digits = {0};
            for (int i = static_cast<int>(bytes.size()) - 1; i >= 0; --i) {
                uint8_t b = bytes[i];
                int carry = b;
                for (size_t j = 0; j < digits.size(); ++j) {
                    int val = digits[j] * 256 + carry;
                    digits[j] = val % 10;
                    carry = val / 10;
                }
                while (carry > 0) {
                    digits.push_back(carry % 10);
                    carry /= 10;
                }
            }
            while (digits.size() > 1 && digits.back() == 0) {
                digits.pop_back();
            }
            std::string s;
            s.reserve(digits.size());
            for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
                s.push_back(static_cast<char>('0' + digits[i]));
            }
            return s;
        }

        static void register_default_methods() {
            static bool initialized = false;
            if (initialized) return;
            initialized = true;

            register_method("current_ms", [](const std::vector<uint8_t>&) -> std::vector<uint8_t> {
                auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count();
                data_block db = data_block::from_int64(static_cast<int64_t>(now));
                return db.bytes;
            });

            register_method("big_uint_to_str", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                std::string s = big_bytes_to_decimal_string(in);
                return data_block::from_string(s).bytes;
            });

            register_method("big_udec_to_str", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                std::string s = big_bytes_to_decimal_string(in);
                return data_block::from_string(s).bytes;
            });

            register_method("big_uint_to_big_udec", [](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                return in;
            });
        }
    };

    class interrupts {
    private:
        std::unordered_map<uint32_t, interrupt_handler> handlers_;
        std::unordered_map<std::string, uint32_t> name_to_id_;
        mutable loaded_methods loaded_methods_;

    public:
        interrupts() {
            register_default_handlers();
        }

        void register_handler(uint32_t id, std::string name, interrupt_handler handler) {
            handlers_[id] = std::move(handler);
            if (!name.empty())
                name_to_id_[name] = id;
        }

        std::vector<uint8_t> invoke(uint32_t id, const std::vector<uint8_t>& param) const {
            auto it = handlers_.find(id);
            if (it == handlers_.end()) {
                assert(false, "Unsupported interrupt ID: ", id);
                return {};
            }
            return it->second(param);
        }

        bool get_id(const std::string& name, uint32_t& out_id) const {
            auto it = name_to_id_.find(name);
            if (it == name_to_id_.end()) return false;
            out_id = it->second;
            return true;
        }

    private:
        void register_default_handlers() {
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
            // 4: load_method
            register_handler(4, "load_method", [this](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                bool ok = loaded_methods_.load(in);
                assert(ok, "load_method failed to find method");
                return {};
            });
            // 5: execute_loaded_method
            register_handler(5, "execute_loaded_method", [this](const std::vector<uint8_t>& in) -> std::vector<uint8_t> {
                bool ok = false;
                auto res = loaded_methods_.execute(in, ok);
                assert(ok, "execute_loaded_method failed: no method loaded or execution failed");
                return res;
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
