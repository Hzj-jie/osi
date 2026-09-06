#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../../../app_info/assert.hpp"
#include "../../../app_info/error_handle.hpp"
#include "../../../automata/typed_node.hpp"
#include "../../code_gen/code_gen.hpp"
#include "../../logic/logic_writer.hpp"
#include "../../logic/builders.hpp"
#include "../scope.hpp"
#include "../bstyle_forward.hpp"

namespace osi
{
    namespace compiler
    {
        namespace bstyle_compiler
        {
            class condition_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "condition"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() >= 5);
                    if (!code_gen_of(n->child(2)).build(o))
                    {
                        return false;
                    }
                    std::string condition;
                    {
                        auto read_target = scope::current()->value_target().primitive_type();
                        if (!read_target.retrieve(condition))
                        {
                            raise_error("Condition of if cannot be a struct.");
                            return false;
                        }
                    }
                    auto satisfied_paragraph = [n](logic_writer& oo) -> bool {
                        return code_gen_of(n->child(4)).build(oo);
                    };
                    if (n->child_count() == 5)
                    {
                        return builders::of_if(condition, satisfied_paragraph).to(o);
                    }
                    auto unsatisfied_paragraph = [n](logic_writer& oo) -> bool {
                        return code_gen_of(n->child(5)).build(oo);
                    };
                    return builders::of_if(condition, satisfied_paragraph, unsatisfied_paragraph).to(o);
                }
            };

            class while_node : public code_gen<logic_writer>
            {
            private:
                bool while_value(const std::shared_ptr<automata::typed_node>& n,
                                 logic_writer& o,
                                 std::string& condition)
                {
                    if (!code_gen_of(n->child(2)).build(o))
                    {
                        return false;
                    }
                    {
                        auto value_target = scope::current()->value_target().primitive_type();
                        if (!value_target.retrieve(condition))
                        {
                            raise_error("Condition of while cannot be a struct.");
                            return false;
                        }
                    }
                    return true;
                }

            public:
                static const char* node_name() { return "while"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    assert(n->child_count() == 5);
                    std::string condition;
                    if (!while_value(n, o, condition))
                    {
                        return false;
                    }
                    return builders::of_while_then(condition, [this, n, &o, &condition]() -> bool {
                        std::string cur_condition;
                        return code_gen_of(n->child(4)).build(o) &&
                               while_value(n, o, cur_condition) &&
                               builders::of_copy(condition, cur_condition).to(o);
                    }).to(o);
                }
            };

            class for_loop_node : public code_gen<logic_writer>
            {
            private:
                struct for_ref
                {
                    std::shared_ptr<automata::typed_node> first;
                    std::shared_ptr<automata::typed_node> second;
                    std::shared_ptr<automata::typed_node> third;
                    std::shared_ptr<automata::typed_node> paragraph;

                    explicit for_ref(const std::shared_ptr<automata::typed_node>& n)
                    {
                        assert(n != nullptr);
                        assert(n->child_count() >= 6 && n->child_count() <= 9);
                        std::vector<std::shared_ptr<automata::typed_node>> m;
                        for (uint32_t i = 0; i < n->child_count(); ++i)
                        {
                            if (n->child(i)->type_name == "semi-colon")
                            {
                                m.push_back(n->child(i));
                            }
                        }
                        assert(m.size() == 2);
                        int idx0 = -1, idx1 = -1;
                        for (uint32_t i = 0; i < n->child_count(); ++i)
                        {
                            if (n->child(i) == m[0]) idx0 = static_cast<int>(i);
                            if (n->child(i) == m[1]) idx1 = static_cast<int>(i);
                        }
                        if (idx0 == 3)
                        {
                            first = n->child(2);
                        }
                        if (idx1 - idx0 == 2)
                        {
                            second = n->child(idx0 + 1);
                        }
                        if (static_cast<int>(n->child_count()) - idx1 == 4)
                        {
                            third = n->child(n->child_count() - 3);
                        }
                        for (uint32_t i = 0; i < n->child_count(); ++i)
                        {
                            if (n->child(i)->type_name == "paragraph")
                            {
                                paragraph = n->child(i);
                                break;
                            }
                        }
                        assert(paragraph != nullptr);
                    }
                };

                bool condition_value(const for_ref& r, logic_writer& o, std::string& condition)
                {
                    if (r.second != nullptr && !code_gen_of(r.second).build(o))
                    {
                        return false;
                    }
                    {
                        auto read_target = scope::current()->value_target().primitive_type();
                        if (!read_target.retrieve(condition))
                        {
                            raise_error("Condition of for-loop cannot be a struct.");
                            return false;
                        }
                    }
                    return true;
                }

            public:
                static const char* node_name() { return "for-loop"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    return builders::start_scope(o).of([this, n, &o]() -> bool {
                        auto sc = scope::current()->start_scope();
                        for_ref ref(n);
                        std::string condition;
                        return (ref.first == nullptr || code_gen_of(ref.first).build(o)) &&
                               condition_value(ref, o, condition) &&
                               builders::of_while_then(condition, [this, &ref, &o, &condition]() -> bool {
                                   std::string cur_condition;
                                   return code_gen_of(ref.paragraph).build(o) &&
                                          (ref.third == nullptr || code_gen_of(ref.third).build(o)) &&
                                          condition_value(ref, o, cur_condition) &&
                                          builders::of_copy(condition, cur_condition).to(o);
                               }).to(o);
                    });
                }
            };

            class multi_sentence_paragraph_node : public code_gen<logic_writer>
            {
            public:
                static const char* node_name() { return "multi-sentence-paragraph"; }

                bool build(const std::shared_ptr<automata::typed_node>& n, logic_writer& o) override
                {
                    assert(n != nullptr);
                    return builders::start_scope(o).of([n, &o]() -> bool {
                        auto sc = scope::current()->start_scope();
                        for (uint32_t i = 1; i < n->child_count() - 1; ++i)
                        {
                            if (!code_gen_of(n->child(i)).build(o))
                            {
                                return false;
                            }
                        }
                        return true;
                    });
                }
            };
        }
    }
}
