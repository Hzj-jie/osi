#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename Writer>
        class statement
        {
        public:
            virtual ~statement() = default;
            virtual void export_to(Writer& o) = 0;
        };

        template <typename Writer>
        class statements
        {
        private:
            std::vector<std::shared_ptr<statement<Writer>>> v_;

        public:
            statements() = default;

            void register_statement(std::shared_ptr<statement<Writer>> p)
            {
                assert(p != nullptr);
                v_.push_back(std::move(p));
            }

            void export_to(Writer& o) const
            {
                for (const auto& s : v_)
                {
                    assert(s != nullptr);
                    s->export_to(o);
                }
            }
        };
    }
}
