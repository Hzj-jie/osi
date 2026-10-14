#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <functional>
#include "../code_gen/statements.hpp"
#include "../logic/logic_writer.hpp"
#include "compiler_scope_interface.hpp"
#include "accessor.hpp"
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        class call_hierarchy_t
        {
        private:
            struct state_t
            {
                const std::string main_name = "main";
                std::unordered_map<std::string, std::vector<std::string>> m_;
                std::unordered_set<std::string> tm_;

                bool can_reach_root(const std::string& f) const
                {
                    return tm_.find(f) != tm_.end();
                }
            };

            std::shared_ptr<state_t> state_;
            std::function<std::string()> current_func_name_fn_;

        public:
            call_hierarchy_t()
                : state_(std::make_shared<state_t>()) {}

            explicit call_hierarchy_t(std::function<std::string()> current_func_name_fn)
                : state_(std::make_shared<state_t>()), current_func_name_fn_(std::move(current_func_name_fn)) {}

            void set_current_function_name_fn(std::function<std::string()> fn)
            {
                current_func_name_fn_ = std::move(fn);
            }

            void to(const std::string& name)
            {
                assert(!name.empty());
                std::string from = state_->main_name;
                if (current_func_name_fn_)
                {
                    std::string cur = current_func_name_fn_();
                    if (!cur.empty()) from = cur;
                }
                else
                {
                    auto* cur = compiler_scope_interface::current_or_null();
                    if (cur)
                    {
                        std::string fn = cur->current_function_name();
                        if (!fn.empty()) from = fn;
                    }
                }
                if (name != from)
                {
                    state_->m_[from].push_back(name);
                }
            }

            void calculate()
            {
                state_->tm_.clear();
                std::queue<std::string> q;
                q.push(state_->main_name);
                state_->tm_.insert(state_->main_name);
                while (!q.empty())
                {
                    std::string f = q.front();
                    q.pop();
                    auto it = state_->m_.find(f);
                    if (it == state_->m_.end()) continue;
                    for (const auto& other : it->second)
                    {
                        if (state_->tm_.insert(other).second)
                        {
                            q.push(other);
                        }
                    }
                }
            }

            bool can_reach_root(const std::string& f) const
            {
                return state_->can_reach_root(f);
            }

            std::function<std::string()> filter(const std::string& f, const std::function<std::string()>& o)
            {
                return [st = state_, f, o]() -> std::string {
                    if (!st->can_reach_root(f))
                    {
                        return "";
                    }
                    return o();
                };
            }

            template <typename Writer = logic_writer>
            class calculator_t : public statement<Writer>
            {
            public:
                void export_to(Writer&) override
                {
                    auto* cur = compiler_scope_interface::current();
                    if (cur && cur->myself().call_hierarchy())
                    {
                        cur->myself().call_hierarchy()->calculate();
                    }
                }

                static void register_statement(statements<Writer>& p)
                {
                    p.register_statement(std::make_shared<calculator_t<Writer>>());
                }
            };

            using calculator = calculator_t<logic_writer>;
        };
    }
}
