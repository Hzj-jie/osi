
#pragma once
#include <atomic>
#include <utility>
#include "../app_info/assert.hpp"

template <typename T>
class exclusive_container
{
private:
    std::atomic<T*> v;
public:
    T* get() const
    {
        return v.load(std::memory_order_consume);
    }

    bool has_value() const
    {
        return get() != nullptr;
    }

    T& ref() const
    {
        assert(has_value());
        return *(get());
    }

    T& operator*() const
    {
        return ref();
    }

    bool clear()
    {
        if(has_value())
        {
            T* o = v.load(std::memory_order_consume);
            if(o == nullptr) return false;
            else
            {
                T* oo = o;
                return v.compare_exchange_weak(o, nullptr, std::memory_order_consume) &&
                       assert(o == oo);
            }
        }
        else return false;
    }

    template <typename... Args>
    bool create(Args&&... args)
    {
        if(has_value()) return false;
        else
        {
            T* o = v.load(std::memory_order_consume);
            if(o == nullptr)
            {
                T* r = new T(std::forward<Args>(args)...);
                if(v.compare_exchange_weak(o, r, std::memory_order_consume) &&
                   assert(o == nullptr)) return true;
                else
                {
                    delete r;
                    return false;
                }
            }
            else return false;
        }
    }
};

