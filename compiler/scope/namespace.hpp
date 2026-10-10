#pragma once
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "../../app_info/assert.hpp"
#include "compiler_scope_interface.hpp"
#include "features.hpp"
#include "accessor.hpp"

namespace osi
{
    namespace compiler
    {
        class current_namespace_t
        {
        private:
            std::vector<std::string> s_;

        public:
            inline static const std::string namespace_separator = "::";

            struct defer_pop
            {
                current_namespace_t* self{nullptr};
                defer_pop() = default;
                explicit defer_pop(current_namespace_t* s) : self(s) {}
                defer_pop(const defer_pop&) = delete;
                defer_pop& operator=(const defer_pop&) = delete;
                defer_pop(defer_pop&& o) noexcept : self(o.self) { o.self = nullptr; }
                defer_pop& operator=(defer_pop&& o) noexcept
                {
                    if (this != &o)
                    {
                        if (self && !self->s_.empty()) self->s_.pop_back();
                        self = o.self;
                        o.self = nullptr;
                    }
                    return *this;
                }
                ~defer_pop()
                {
                    if (self && !self->s_.empty())
                    {
                        self->s_.pop_back();
                    }
                }
            };

            std::shared_ptr<defer_pop> define(const std::string& name)
            {
                assert(!name.empty());
                assert(name.rfind(namespace_separator, 0) == 0);
                s_.push_back(name);
                return std::make_shared<defer_pop>(this);
            }

            static std::string of(const std::string& i)
            {
                assert(!i.empty());
                auto* cur = compiler_scope_interface::current_or_null();
                if (cur != nullptr && !cur->features().with_namespace())
                {
                    return i;
                }
                if (i.rfind(namespace_separator, 0) == 0)
                {
                    return i;
                }
                if (cur != nullptr)
                {
                    auto* ns = cur->myself().current_namespace();
                    if (ns != nullptr && !ns->s_.empty())
                    {
                        return ns->s_.back() + namespace_separator + i;
                    }
                }
                return namespace_separator + i;
            }

            static std::pair<std::string, std::string> of_namespace_and_name(const std::string& i)
            {
                std::string f = of(i);
                size_t index = f.rfind(namespace_separator);
                if (index == std::string::npos)
                {
                    return {"", f};
                }
                return {f.substr(0, index), f.substr(index + namespace_separator.length())};
            }

            const std::vector<std::string>& stack() const { return s_; }
            std::vector<std::string>& stack() { return s_; }
        };

        class namespace_t
        {
        public:
            static std::string fully_qualified_name(const std::string& n)
            {
                assert(!n.empty());
                auto* cur = compiler_scope_interface::current_or_null();
                if (cur != nullptr && !cur->features().with_namespace())
                {
                    return n;
                }
                if (n.rfind(current_namespace_t::namespace_separator, 0) == 0)
                {
                    return n;
                }
                return current_namespace_t::namespace_separator + n;
            }

            static std::string fully_qualified_name(const std::string& n, const std::string& i)
            {
                assert(!i.empty());
                auto* cur = compiler_scope_interface::current_or_null();
                if (cur != nullptr && !cur->features().with_namespace())
                {
                    return i;
                }
                if (i.rfind(current_namespace_t::namespace_separator, 0) == 0)
                {
                    return i;
                }
                if (n.empty())
                {
                    return fully_qualified_name(i);
                }
                return fully_qualified_name(n + current_namespace_t::namespace_separator + i);
            }
        };
    }
}
