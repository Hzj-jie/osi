#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <algorithm>
#include "../app_info/assert.hpp"
#include "../app_info/error_handle.hpp"
#include "../utils/strutils.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class syntax;
        }

        class syntax_collection
        {
        private:
            struct impl
            {
                std::unordered_map<std::string, uint32_t> token_str_type;
                std::vector<std::string> str_token_type;
                std::unordered_map<std::string, uint32_t> syntax_str_type;
                std::vector<std::string> str_syntax_type;
                std::vector<std::shared_ptr<syntax::syntax>> v;

                impl() = default;

                explicit impl(const std::unordered_map<std::string, uint32_t>& token_map)
                    : token_str_type(token_map)
                {
                    std::vector<std::pair<std::string, uint32_t>> pairs(token_map.begin(), token_map.end());
                    std::sort(pairs.begin(), pairs.end(), [](const auto& a, const auto& b) {
                        return a.second < b.second;
                    });
                    str_token_type.resize(pairs.size());
                    for (size_t i = 0; i < pairs.size(); ++i)
                    {
                        str_token_type[i] = pairs[i].first;
                    }
                }
            };

            std::shared_ptr<impl> impl_;

        public:
            syntax_collection() : impl_(std::make_shared<impl>()) {}

            explicit syntax_collection(const std::unordered_map<std::string, uint32_t>& token_map)
                : impl_(std::make_shared<impl>(token_map)) {}

            uint32_t next_type() const
            {
                return static_cast<uint32_t>(impl_->str_token_type.size() + impl_->str_syntax_type.size());
            }

            void clear()
            {
                impl_->syntax_str_type.clear();
                impl_->str_syntax_type.clear();
                impl_->v.clear();
            }

            bool token_type(const std::string& name, uint32_t& o) const
            {
                auto it = impl_->token_str_type.find(name);
                if (it != impl_->token_str_type.end())
                {
                    o = it->second;
                    return true;
                }
                return false;
            }

            bool syntax_type(const std::string& name, uint32_t& o) const
            {
                auto it = impl_->syntax_str_type.find(name);
                if (it != impl_->syntax_str_type.end())
                {
                    o = it->second;
                    return true;
                }
                return false;
            }

            bool type_token(uint32_t id, std::string& o) const
            {
                if (id < impl_->str_token_type.size())
                {
                    o = impl_->str_token_type[id];
                    return true;
                }
                return false;
            }

            bool type_syntax(uint32_t id, std::string& o) const
            {
                if (id >= impl_->str_token_type.size() && id < next_type())
                {
                    o = impl_->str_syntax_type[id - impl_->str_token_type.size()];
                    return true;
                }
                return false;
            }

            bool type_id(const std::string& name, uint32_t& o) const
            {
                return token_type(name, o) || syntax_type(name, o);
            }

            uint32_t type_id(const std::string& name) const
            {
                uint32_t o = 0;
                assert(type_id(name, o));
                return o;
            }

            bool type_name(uint32_t id, std::string& o) const
            {
                return type_token(id, o) || type_syntax(id, o);
            }

            std::string type_name(uint32_t id) const
            {
                std::string o;
                if (type_name(id, o)) return o;
                return strcat("UNDEFINED_TYPE-", id);
            }

            bool define(const std::string& name, uint32_t& o)
            {
                uint32_t dummy = 0;
                assert(!token_type(name, dummy));
                auto it = impl_->syntax_str_type.find(name);
                if (it == impl_->syntax_str_type.end())
                {
                    o = next_type();
                    impl_->syntax_str_type[name] = o;
                    impl_->str_syntax_type.push_back(name);
                }
                else
                {
                    o = it->second;
                }
                return true;
            }

            uint32_t define(const std::string& name)
            {
                uint32_t o = 0;
                assert(define(name, o));
                return o;
            }

            bool set(uint32_t type, std::shared_ptr<syntax::syntax> s)
            {
                if (s == nullptr) return false;
                if (impl_->v.size() <= type)
                {
                    impl_->v.resize(type + 1);
                }
                if (impl_->v[type] == nullptr)
                {
                    impl_->v[type] = s;
                    return true;
                }
                return false;
            }

            bool get(uint32_t type, std::shared_ptr<syntax::syntax>& o) const
            {
                if (type < impl_->v.size() && impl_->v[type] != nullptr)
                {
                    o = impl_->v[type];
                    return true;
                }
                return false;
            }

            bool get(const std::vector<uint32_t>& types, std::vector<std::shared_ptr<syntax::syntax>>& o) const
            {
                o.clear();
                o.reserve(types.size());
                for (uint32_t t : types)
                {
                    std::shared_ptr<syntax::syntax> s;
                    if (!get(t, s)) return false;
                    o.push_back(s);
                }
                return true;
            }

            std::vector<std::shared_ptr<syntax::syntax>> get(const std::vector<uint32_t>& types) const
            {
                std::vector<std::shared_ptr<syntax::syntax>> o;
                assert(get(types, o));
                return o;
            }

            bool complete() const
            {
                for (const auto& pair : impl_->syntax_str_type)
                {
                    std::shared_ptr<syntax::syntax> dummy;
                    if (!get(pair.second, dummy))
                    {
                        raise_error("type ", pair.first, " has not been defined.");
                        return false;
                    }
                }
                return true;
            }

            const std::unordered_map<std::string, uint32_t>& token_map() const { return impl_->token_str_type; }
            const std::unordered_map<std::string, uint32_t>& syntax_map() const { return impl_->syntax_str_type; }
        };
    }
}
