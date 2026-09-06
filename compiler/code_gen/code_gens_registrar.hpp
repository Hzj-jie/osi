#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "code_gen.hpp"
#include "code_gen.default.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename Writer>
        class code_gens_registrar
        {
        public:
            using action_t = std::function<void(code_gens<Writer>&)>;

        private:
            std::vector<action_t> actions_;

        public:
            code_gens_registrar() = default;

            code_gens_registrar& with(action_t a)
            {
                assert(a != nullptr);
                actions_.push_back(std::move(a));
                return *this;
            }

            code_gens_registrar& with(const std::vector<action_t>& actions)
            {
                for (const auto& a : actions)
                {
                    assert(a != nullptr);
                    actions_.push_back(a);
                }
                return *this;
            }

            code_gens_registrar& with(const std::string& name, std::shared_ptr<code_gen<Writer>> gen)
            {
                assert(!name.empty());
                assert(gen != nullptr);
                return with([name, gen = std::move(gen)](code_gens<Writer>& cg) {
                    cg.register_gen(name, gen);
                });
            }

            template <typename T, typename... Args>
            code_gens_registrar& with(Args&&... args)
            {
                auto gen = std::make_shared<T>(std::forward<Args>(args)...);
                return with(code_gens<Writer>::template code_gen_name<T>(), std::move(gen));
            }

            template <typename T, typename... Args>
            code_gens_registrar& with_named(const std::string& name, Args&&... args)
            {
                auto gen = std::make_shared<T>(std::forward<Args>(args)...);
                return with(name, std::move(gen));
            }

            code_gens_registrar& with_delegate(
                const std::string& name,
                std::function<bool(const std::shared_ptr<automata::typed_node>&, Writer&)> f)
            {
                return with(code_gen_delegate<Writer>::of(name, std::move(f)));
            }

            code_gens_registrar& with_delegate(
                const std::string& name,
                typename code_gen_delegate<Writer>::full_func_t f)
            {
                return with(code_gen_delegate<Writer>::of(name, std::move(f)));
            }

            code_gens_registrar& with_of_only_childs(const std::vector<std::string>& names)
            {
                for (const auto& name : names)
                {
                    with(code_gen_default::template of_only_child<Writer>(name));
                }
                return *this;
            }

            code_gens_registrar& with_of_all_childrens(const std::vector<std::string>& names)
            {
                for (const auto& name : names)
                {
                    with(code_gen_default::template of_all_children<Writer>(name));
                }
                return *this;
            }

            const std::vector<action_t>& actions() const
            {
                return actions_;
            }

            void apply(code_gens<Writer>& cg) const
            {
                for (const auto& a : actions_)
                {
                    a(cg);
                }
            }
        };
    }
}
