#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "matching.hpp"
#include "empty_matching.hpp"
#include "matching_delegate.hpp"

namespace osi
{
    namespace automata
    {
        namespace syntax
        {
            class syntax : public matching
            {
            public:
                uint32_t type{0};
                std::vector<std::shared_ptr<matching>> ms;

                syntax(syntax_collection& col, uint32_t t, std::vector<std::shared_ptr<matching>> matchers)
                    : matching(col), type(t), ms(std::move(matchers))
                {
                    assert(!ms.empty());
                }

                result match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p) override
                {
                    if (v == nullptr || p >= v->size())
                    {
                        return result::failure(p);
                    }

                    return disallow_cycle_dependency(type, p, [&]() {
                        std::vector<std::shared_ptr<typed_node>> nodes;
                        uint32_t op = p;
                        for (size_t i = 0; i < ms.size(); ++i)
                        {
                            result r = ms[i]->match(v, p);
                            if (r.failed())
                            {
                                return r;
                            }
                            assert(r.suc.has_value());
                            p = r.suc->pos;
                            nodes.insert(nodes.end(), r.suc->nodes.begin(), r.suc->nodes.end());
                        }
                        auto root = create_node(v, type, op, p);
                        root->attach(nodes);
                        return result::success(p, root);
                    });
                }

                static bool create(uint32_t type, const std::string& s, syntax_collection& collection, std::shared_ptr<syntax>& o);
                static bool create(const std::string& type_name, const std::string& s, syntax_collection& collection);
            };

            inline matching::result matching_delegate::match(const std::shared_ptr<const std::vector<typed_word>>& v, uint32_t p)
            {
                std::shared_ptr<syntax> s;
                assert(c.get(type_, s) && s != nullptr);
                return s->match(v, p);
            }
        }
    }
}
