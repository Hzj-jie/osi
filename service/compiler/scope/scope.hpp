#pragma once
#include <functional>
#include <vector>
#include <memory>
#include "../../../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename T>
        class scope_base
        {
        protected:
            inline static thread_local T* in_thread{nullptr};
            T* parent{nullptr};
            T* child{nullptr};
            std::vector<std::function<void()>> ds;

            explicit scope_base(T* parent_ptr)
                : parent(parent_ptr)
            {
                if (parent == nullptr)
                {
                    assert(in_thread == nullptr);
                }
                else
                {
                    assert(in_thread == parent);
                    assert(parent->child == nullptr);
                    parent->child = static_cast<T*>(this);
                }
                in_thread = static_cast<T*>(this);
            }

        public:
            virtual ~scope_base()
            {
                assert(in_thread == static_cast<T*>(this));
                for (auto it = ds.rbegin(); it != ds.rend(); ++it)
                {
                    if (*it) (*it)();
                }
                if (parent != nullptr)
                {
                    assert(parent->child == static_cast<T*>(this));
                    parent->child = nullptr;
                }
                in_thread = parent;
            }

            scope_base(const scope_base&) = delete;
            scope_base& operator=(const scope_base&) = delete;
            scope_base(scope_base&&) = delete;
            scope_base& operator=(scope_base&&) = delete;

            void when_end_scope(std::function<void()> a)
            {
                assert(static_cast<bool>(a));
                ds.push_back(std::move(a));
            }

            void disable_end_scope()
            {
                ds.clear();
            }

            bool is_root() const
            {
                return parent == nullptr;
            }

            static T* current()
            {
                assert(in_thread != nullptr);
                return in_thread;
            }

            static T* current_or_null()
            {
                return in_thread;
            }

            T* get_parent() const
            {
                return parent;
            }

            T* get_root()
            {
                T* s = static_cast<T*>(this);
                while (!s->is_root())
                {
                    s = s->parent;
                }
                return s;
            }

            const T* get_root() const
            {
                const T* s = static_cast<const T*>(this);
                while (!s->is_root())
                {
                    s = s->parent;
                }
                return s;
            }

            template <typename Getter>
            auto from_root(Getter&& getter) -> decltype(getter(static_cast<T*>(nullptr)))
            {
                return getter(get_root());
            }

            template <typename Getter>
            auto from_root(Getter&& getter) const -> decltype(getter(static_cast<const T*>(nullptr)))
            {
                return getter(get_root());
            }
        };
    }
}
