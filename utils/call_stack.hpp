
#pragma once
#include <boost/predef.h>
#include <utility>
#include <stdlib.h>
#include "../app_info/assert.hpp"
#include "../template/singleton.hpp"
#include "../envs/os.hpp"
#include <type_traits>
#include "../utils/macro.hpp"
#include "../threadpool/thread_local.hpp"

namespace __call_stack_private
{
    const static size_t default_max_size = UINT8_MAX;
}

template <typename T, size_t MAX_SIZE = __call_stack_private::default_max_size>
class call_stack_t
{
private:
#if BOOST_COMP_MSVC
    static fixed_stack<T, MAX_SIZE>& c()
    {
        static thread_local_storage<fixed_stack<T, MAX_SIZE>> tls;
        return (*tls);
    }
#else
    THREAD_LOCAL_DEFAULT_SINGLETON_FUNC(SINGLE_ARG(fixed_stack<T, MAX_SIZE>), c);
#endif
    call_stack_t() = default;

public:
    T& current() const
    {
        return c().back();
    }

    template <typename U>
    void push(U&& i) const
    {
        assert(c().push(std::forward<U>(i)));
    }

    void pop() const
    {
        T o;
        assert(c().pop(o));
    }

    CONST_SINGLETON(call_stack_t);
};

template <typename T, size_t MAX_SIZE = __call_stack_private::default_max_size>
const static call_stack_t<T, MAX_SIZE>& call_stack()
{
    return call_stack_t<T, MAX_SIZE>::instance();
}

