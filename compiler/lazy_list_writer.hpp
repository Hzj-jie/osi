#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <sstream>
#include "../app_info/assert.hpp"
#include "../interpreter/primitive/data_block.hpp"

namespace osi
{
    namespace compiler
    {
        class lazy_list_writer
        {
        protected:
            std::vector<std::function<std::string()>> v_;

        public:
            lazy_list_writer() = default;
            virtual ~lazy_list_writer() = default;

            bool append(const std::string& s)
            {
                if (s.empty()) return true;
                v_.emplace_back([s]() { return s; });
                return true;
            }

            bool append(const char* s)
            {
                assert(s != nullptr);
                return append(std::string(s));
            }

            bool append(char c)
            {
                return append(std::string(1, c));
            }

            bool append(uint32_t val)
            {
                return append(std::to_string(val));
            }

            bool append(const primitive::data_block& d)
            {
                v_.emplace_back([d]() { return d.to_assembly_string(); });
                return true;
            }

            bool append(const std::vector<std::string>& v)
            {
                v_.emplace_back([v]() {
                    std::string res;
                    for (size_t i = 0; i < v.size(); ++i)
                    {
                        if (i > 0) res += ' ';
                        res += v[i];
                    }
                    return res;
                });
                return true;
            }

            bool append(const std::vector<std::pair<std::string, std::string>>& v)
            {
                v_.emplace_back([v]() {
                    std::string res;
                    for (size_t i = 0; i < v.size(); ++i)
                    {
                        if (i > 0) res += ' ';
                        res += v[i].first;
                        res += ' ';
                        res += v[i].second;
                    }
                    return res;
                });
                return true;
            }

            bool append(std::function<std::string()> f)
            {
                assert(f != nullptr);
                v_.emplace_back(std::move(f));
                return true;
            }

            template <typename Writer>
            bool append(const std::function<bool(Writer&)>& a)
            {
                assert(a != nullptr);
                return a(static_cast<Writer&>(*this));
            }

            bool append(const lazy_list_writer& w)
            {
                v_.emplace_back([w]() { return w.str(); });
                return true;
            }

            bool append(const std::shared_ptr<lazy_list_writer>& w)
            {
                assert(w != nullptr);
                v_.emplace_back([w]() { return w->str(); });
                return true;
            }

            std::string str() const
            {
                std::string res;
                for (size_t i = 0; i < v_.size(); ++i)
                {
                    if (i > 0)
                    {
                        res += ' ';
                    }
                    res += v_[i]();
                }
                return res;
            }

            std::string ToString() const
            {
                return str();
            }

            std::string dump() const
            {
                return str();
            }
        };
    }
}
