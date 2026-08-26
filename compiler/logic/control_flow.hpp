#pragma once
#include <string>
#include <vector>
#include <memory>
#include "instruction_gen.hpp"
#include "scope.hpp"
#include "variable.hpp"

namespace osi
{
    namespace compiler
    {
        namespace logic
        {
            class paragraph : public instruction_gen
            {
            public:
                std::vector<std::shared_ptr<instruction_gen>> s;

                paragraph() = default;

                template <typename... Args>
                explicit paragraph(Args&&... args)
                {
                    (push(std::forward<Args>(args)), ...);
                }

                bool push(std::shared_ptr<instruction_gen> e)
                {
                    if (!e) return false;
                    s.push_back(std::move(e));
                    return true;
                }

                bool build(std::vector<std::string>& o) const override
                {
                    scope child_scope(scope::current());
                    for (const auto& stmt : s)
                    {
                        assert(stmt != nullptr);
                        if (!stmt->build(o)) return false;
                    }
                    return true;
                }
            };

            class _start_scope : public instruction_gen
            {
            public:
                std::shared_ptr<paragraph> p;

                explicit _start_scope(std::shared_ptr<paragraph> p) : p(std::move(p))
                {
                    assert(this->p != nullptr);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    return p->build(o);
                }
            };

            class _if : public instruction_gen
            {
            private:
                std::string v;
                std::shared_ptr<paragraph> true_path;
                std::shared_ptr<paragraph> false_path;

            public:
                _if(std::string v, std::shared_ptr<paragraph> true_path, std::shared_ptr<paragraph> false_path = nullptr)
                    : v(std::move(v)), true_path(std::move(true_path)), false_path(std::move(false_path))
                {
                    assert(!this->v.empty());
                    assert(this->true_path != nullptr);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable var;
                    if (!variable::of(v, o, var)) return false;

                    std::vector<std::string> true_o;
                    if (!true_path->build(true_o)) return false;

                    std::vector<std::string> false_o;
                    if (false_path && !false_path->build(false_o)) return false;

                    false_o.push_back(strcat("jump rel", true_o.size() + 1));
                    o.push_back(strcat("jumpif rel", false_o.size() + 1, " ", var.ToString()));
                    o.insert(o.end(), false_o.begin(), false_o.end());
                    o.insert(o.end(), true_o.begin(), true_o.end());
                    return true;
                }
            };

            class _while_then : public instruction_gen
            {
            private:
                std::string v;
                std::shared_ptr<paragraph> p;

            public:
                _while_then(std::string v, std::shared_ptr<paragraph> p)
                    : v(std::move(v)), p(std::move(p))
                {
                    assert(!this->v.empty());
                    assert(this->p != nullptr);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    variable var;
                    if (!variable::of(v, o, var)) return false;

                    std::vector<std::string> po;
                    if (!p->build(po)) return false;

                    o.push_back(strcat("jumpif rel2 ", var.ToString()));
                    o.push_back(strcat("jump rel", po.size() + 2));
                    o.insert(o.end(), po.begin(), po.end());
                    o.push_back(strcat("jump rel-", po.size() + 2));
                    return true;
                }
            };

            class _do_while : public instruction_gen
            {
            private:
                std::string v;
                std::shared_ptr<paragraph> p;

            public:
                _do_while(std::string v, std::shared_ptr<paragraph> p)
                    : v(std::move(v)), p(std::move(p))
                {
                    assert(!this->v.empty());
                    assert(this->p != nullptr);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    uint32_t start = static_cast<uint32_t>(o.size());
                    if (!p->build(o)) return false;

                    variable var;
                    if (!variable::of(v, o, var)) return false;

                    int64_t diff = static_cast<int64_t>(start) - static_cast<int64_t>(o.size());
                    o.push_back(strcat("jumpif rel", diff, " ", var.ToString()));
                    return true;
                }
            };

            class _do_until : public instruction_gen
            {
            private:
                std::string v;
                std::shared_ptr<paragraph> p;

            public:
                _do_until(std::string v, std::shared_ptr<paragraph> p)
                    : v(std::move(v)), p(std::move(p))
                {
                    assert(!this->v.empty());
                    assert(this->p != nullptr);
                }

                bool build(std::vector<std::string>& o) const override
                {
                    uint32_t start = static_cast<uint32_t>(o.size());
                    if (!p->build(o)) return false;

                    variable var;
                    if (!variable::of(v, o, var)) return false;

                    int64_t diff = static_cast<int64_t>(start) - static_cast<int64_t>(o.size()) - 1;
                    o.push_back(strcat("jumpif rel2 ", var.ToString()));
                    o.push_back(strcat("jump rel", diff));
                    return true;
                }
            };
        }
    }
}
