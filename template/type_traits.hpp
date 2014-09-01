
#pragma once
#include <type_traits>
#include "../utils/macro.hpp"

namespace __type_traits_private
{
    class Yes_t;
    class No_t;

    template <typename T>
    struct yes
    {
        const static bool value = (std::is_same<T, Yes_t&>::value) ||
                                  (std::is_same<T, Yes_t>::value);
    };

    template <typename T>
    struct no
    {
        const static bool value = (std::is_same<T, No_t&>::value) ||
                                  (std::is_same<T, No_t>::value);
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
                static __type_traits_private::Yes_t& test(type_check<type_check_parameters>*); \
                template <typename> \
                static __type_traits_private::No_t& test(...); \
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

namespace __type_traits_operator_overloads_private
{
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator==(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator==(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator==(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator==(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator!=(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator!=(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator!=(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator!=(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<=(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<=(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<=(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<=(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>=(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>=(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>=(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>=(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<<(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<<(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<<(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator<<(T1&, T2&);

    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>>(const T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>>(const T1&, T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>>(T1&, const T2&);
    template <typename T1, typename T2>
    __type_traits_private::No_t& operator>>(T1&, T2&);

    template <typename T1, typename T2>
    struct operator_overloads
    {
    public:
#define HAS_OPERATOR_TEMPLATE(name, op) \
                const static bool has_operator_##name##_const_const = \
                    !__type_traits_private::no< \
                        decltype(const_cast<const T1>(*(T1*)(0)) op \
                                 const_cast<const T2>(*(T2*)(0)))>::value; \
                const static bool has_operator_##name##_variant_const = \
                    !__type_traits_private::no< \
                        decltype(const_cast<const T1>(*(T1*)(0)) op \
                                 const_cast<const T2>(*(T2*)(0)))>::value; \
                const static bool has_operator_##name##_const_variant = \
                    !__type_traits_private::no< \
                        decltype(const_cast<const T1>(*(T1*)(0)) op \
                                 const_cast<const T2>(*(T2*)(0)))>::value; \
                const static bool has_operator_##name##_variant_variant = \
                    !__type_traits_private::no< \
                        decltype(const_cast<const T1>(*(T1*)(0)) op \
                                 const_cast<const T2>(*(T2*)(0)))>::value;
        HAS_OPERATOR_TEMPLATE(equal, ==);
        HAS_OPERATOR_TEMPLATE(not_equal, !=);
        HAS_OPERATOR_TEMPLATE(less, <);
        HAS_OPERATOR_TEMPLATE(less_or_equal, <=);
        HAS_OPERATOR_TEMPLATE(more, >);
        HAS_OPERATOR_TEMPLATE(more_or_equal, >=);
        HAS_OPERATOR_TEMPLATE(left_shift, <<);
        HAS_OPERATOR_TEMPLATE(right_shift, >>);
#undef HAS_OPERATOR_TEMPLATE
    };
}

template <typename T1, typename T2 = T1>
struct operator_overloads : __type_traits_operator_overloads_private::operator_overloads<T1, T2> { };

