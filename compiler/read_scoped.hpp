#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "../app_info/assert.hpp"

namespace osi
{
    namespace compiler
    {
        template <typename T>
        class read_scoped
        {
        private:
            std::vector<T> s_;
            uint32_t pending_dispose_{0};

        public:
            class ref
            {
            private:
                read_scoped<T>* r_{nullptr};
                T current_{};

            public:
                ref() = default;

                explicit ref(read_scoped<T>* r)
                    : r_(r)
                {
                    assert(r_ != nullptr);
                    assert(!r_->s_.empty());
                    r_->pending_dispose_++;
                    current_ = r_->s_.back();
                }

                ~ref()
                {
                    reset();
                }

                void reset()
                {
                    if (r_ != nullptr)
                    {
                        assert(r_->pending_dispose_ > 0);
                        r_->pending_dispose_--;
                        assert(!r_->s_.empty());
                        r_->s_.pop_back();
                        r_ = nullptr;
                    }
                }

                ref(const ref&) = delete;
                ref& operator=(const ref&) = delete;

                ref(ref&& other) noexcept
                    : r_(other.r_), current_(std::move(other.current_))
                {
                    other.r_ = nullptr;
                }

                ref& operator=(ref&& other) noexcept
                {
                    if (this != &other)
                    {
                        reset();
                        r_ = other.r_;
                        current_ = std::move(other.current_);
                        other.r_ = nullptr;
                    }
                    return *this;
                }

                const T& get() const { return current_; }
                const T& operator*() const { return current_; }
                const T* operator->() const { return &current_; }
                T& get() { return current_; }
                T& operator*() { return current_; }
                T* operator->() { return &current_; }
            };

            void push(T v)
            {
                s_.push_back(std::move(v));
            }

            ref pop()
            {
                assert(size() > pending_dispose_);
                return ref(this);
            }

            uint32_t size() const
            {
                return static_cast<uint32_t>(s_.size());
            }

            bool empty() const
            {
                return s_.empty();
            }
        };
    }
}
