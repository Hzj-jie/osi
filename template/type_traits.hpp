
#pragma once
#include <type_traits>
#include "../utils/macro.hpp"

namespace __type_traits_private
{
    class Yes_t { };
    class No_t { };

    template <typename T>
    struct yes
    {
        const static bool value = std::is_same<T, Yes_t>::value;
    };
}

#ifdef __PRIVATE_HAS_DEF
    __PRIVATE_HAS_DEF redefined
#endif
#define __PRIVATE_HAS_DEF(name, TYPENAMES, BEGIN, type_check_typenames, type_check_parameters) \
            template <TYPENAMES> \
            struct name { \
                BEGIN; \
                template <type_check_typenames> \
                struct type_check; \
                template <typename U> \
                static __type_traits_private::Yes_t test(type_check<type_check_parameters>*); \
                template <typename> \
                static __type_traits_private::No_t test(...); \
                const static bool value = __type_traits_private::yes<decltype(test<T>(nullptr))>::value; };

#ifdef HAS_TYPEDEF
    HAS_TYPEDEF redefined
#endif
#define HAS_TYPEDEF(typedef_name) __PRIVATE_HAS_DEF(has_typedef_##typedef_name, \
                                                    typename T, \
                                                    , \
                                                    typename U, \
                                                    U::typedef_name);

#ifdef HAS_MEMBER_FUNCTION
    HAS_MEMBER_FUNCTION redefined
#endif
#define HAS_MEMBER_FUNCTION(func) __PRIVATE_HAS_DEF(has_member_function_##func, \
                                                    typename T COMMA typename RET_T COMMA typename... Args, \
                                                    typedef RET_T (T::*Sign)(Args&&...), \
                                                    typename U COMMA U, \
                                                    Sign COMMA &U::func);

#ifdef HAS_CONST_MEMBER_FUNCTION
    HAS_CONST_MEMBER_FUNCTION redefined
#endif
#define HAS_CONST_MEMBER_FUNCTION(func) __PRIVATE_HAS_DEF(has_const_member_function_##func, \
                                                          typename T COMMA typename RET_T COMMA typename... Args, \
                                                          typedef RET_T (T::*Sign)(Args&&...) const, \
                                                          typename U COMMA U, \
                                                          Sign COMMA &U::func);

