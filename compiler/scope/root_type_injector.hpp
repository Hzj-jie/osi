#pragma once
#include <vector>
#include <memory>
#include "../logic/logic_writer.hpp"
#include "../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        class root_type_injector_t
        {
        private:
            std::vector<std::shared_ptr<logic_writer>> stack_;

        public:
            struct guard
            {
                root_type_injector_t* self{nullptr};
                guard() = default;
                explicit guard(root_type_injector_t* s) : self(s) {}
                ~guard()
                {
                    if (self && !self->stack_.empty())
                    {
                        self->stack_.pop_back();
                    }
                }
                guard(const guard&) = delete;
                guard& operator=(const guard&) = delete;
                guard(guard&& o) noexcept : self(o.self) { o.self = nullptr; }
                guard& operator=(guard&& o) noexcept
                {
                    if (this != &o)
                    {
                        if (self && !self->stack_.empty()) self->stack_.pop_back();
                        self = o.self;
                        o.self = nullptr;
                    }
                    return *this;
                }
            };

            [[nodiscard]] std::shared_ptr<guard> _new(logic_writer& o)
            {
                auto w = std::make_shared<logic_writer>();
                stack_.push_back(w);
                o.append(w);
                return std::make_shared<guard>(this);
            }

            std::shared_ptr<logic_writer> current()
            {
                assert(!stack_.empty());
                return stack_.back();
            }
        };
    }
}
