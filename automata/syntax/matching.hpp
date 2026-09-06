#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include "../../app_info/assert.hpp"
#include "../../app_info/error_handle.hpp"
#include "../typed_word.hpp"
#include "../typed_node.hpp"
#include "../syntax_collection.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class matching
            {
            public:
                struct result
                {
                    struct suc_t
                    {
                        uint32_t pos{0};
                        std::vector<std::shared_ptr<typed_node>> nodes;

                        suc_t(uint32_t p, std::vector<std::shared_ptr<typed_node>> n)
                            : pos(p), nodes(std::move(n))
                        {
                        }

                        suc_t operator|(const suc_t& other) const
                        {
                            if (this->pos >= other.pos)
                            {
                                return *this;
                            }
                            return other;
                        }
                    };

                    struct fal_t
                    {
                        uint32_t pos{0};

                        explicit fal_t(uint32_t p = 0) : pos(p) {}

                        fal_t operator|(const fal_t& other) const
                        {
                            return fal_t(std::max(this->pos, other.pos));
                        }
                    };

                    std::optional<suc_t> suc;
                    fal_t fal{0};

                    result() = default;
                    result(std::optional<suc_t> s, fal_t f) : suc(std::move(s)), fal(f) {}

                    bool succeeded() const { return suc.has_value(); }
                    bool failed() const { return !succeeded(); }

                    static result failure(uint32_t p)
                    {
                        return result(std::nullopt, fal_t(p));
                    }

                    static result success(uint32_t p, std::vector<std::shared_ptr<typed_node>> nodes)
                    {
                        return result(suc_t(p, std::move(nodes)), fal_t(0));
                    }

                    static result success(uint32_t p, const std::shared_ptr<typed_node>& node)
                    {
                        return success(p, std::vector<std::shared_ptr<typed_node>>{node});
                    }

                    static result success(uint32_t p)
                    {
                        return success(p, std::vector<std::shared_ptr<typed_node>>{});
                    }

                    result operator|(const result& other) const
                    {
                        if (!this->succeeded() && !other.succeeded())
                        {
                            return result(std::nullopt, this->fal | other.fal);
                        }
                        if (!this->succeeded())
                        {
                            return result(other.suc, this->fal | other.fal);
                        }
                        if (!other.succeeded())
                        {
                            return result(this->suc, this->fal | other.fal);
                        }
                        return result(*this->suc | *other.suc, this->fal | other.fal);
                    }
                };

            protected:
                syntax_collection c;

                std::shared_ptr<typed_node> create_node(const std::shared_ptr<const std::vector<typed_word>>& v,
                                                        uint32_t type,
                                                        uint32_t start,
                                                        uint32_t end) const
                {
                    return std::make_shared<typed_node>(v, type, c.type_name(type), start, end);
                }

                template <typename Func>
                result disallow_cycle_dependency(uint32_t type, uint32_t pos, Func&& f) const
                {
                    thread_local static std::unordered_map<uint32_t, uint32_t> cycle_map;
                    auto it = cycle_map.find(type);
                    if (it != cycle_map.end() && it->second == pos)
                    {
                        raise_error("Cycle dependency found at ", c.type_name(type));
                        return result::failure(pos);
                    }
                    std::optional<uint32_t> prev;
                    if (it != cycle_map.end())
                    {
                        prev = it->second;
                    }
                    cycle_map[type] = pos;

                    struct defer_t
                    {
                        uint32_t t;
                        std::optional<uint32_t> p;
                        ~defer_t()
                        {
                            if (p.has_value())
                            {
                                cycle_map[t] = *p;
                            }
                            else
                            {
                                cycle_map.erase(t);
                            }
                        }
                    } defer_cleanup{type, prev};

                    return f();
                }

            public:
                explicit matching(syntax_collection col) : c(std::move(col)) {}
                virtual ~matching() = default;

                virtual result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) = 0;
            };
        }
    }
}
