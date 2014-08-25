
#pragma once
#include <stdlib.h>
#include "../app_info/assert.hpp"
#include "../template/singleton.hpp"
#include "../envs/os.hpp"

namespace __call_stack_private
{
    const static size_t default_max_size = UINT8_MAX;
}

template <typename T, size_t MAX_SIZE = __call_stack_private::default_max_size>
class call_stack_t
{
private:
    thread_local static fixed_stack<T, MAX_SIZE> c;

    call_stack() = default;

public:
    static T& current()
    {
        return c.back();
    }

    template <typename U>
    static void push(U&& i)
    {
        c.push(i);
    }

    static void pop()
    {
        T o;
        assert(c.pop(o));
    }

    CONST_SINGLETON(call_stack_t);
};

template <typename T, size_t MAX_SIZE = __call_stack_private::default_max_size>
static call_stack_t<T, MAX_SIZE>& call_stack()
{
    return call_stack_t<T, MAX_SIZE>::instance();
}

