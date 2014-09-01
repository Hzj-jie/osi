
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

template <typename T1, typename T2 = T1>
struct operator_overloads
{
private:
    template <typename T>
    struct type_check;
#define HAS_OPERATOR_TEMPLATE_SINGLE(name, op, CC1, CC2, V1, V2) \
        private: \
            template <typename U1, typename U2> \
            static __type_traits_private::Yes_t& name##_##CC1##_##CC2##_test( \
                       type_check<decltype(*(V1 U1*)(0) op *(V2 U2*)(0))>*); \
            template <typename, typename> \
            static __type_traits_private::No_t& name##_##CC1##_##CC2##_test(...); \
        public: \
            const static bool has_operator_##name##_##CC1##_##CC2 = \
                __type_traits_private::yes< \
                    decltype(name##_##CC1##_##CC2##_test<T1, T2>(nullptr))>::value;
#define HAS_OPERATOR_TEMPLATE(name, op) \
        HAS_OPERATOR_TEMPLATE_SINGLE(name, op, const, const, const, const); \
        HAS_OPERATOR_TEMPLATE_SINGLE(name, op, const, variant, const,); \
        HAS_OPERATOR_TEMPLATE_SINGLE(name, op, variant, const, , const); \
        HAS_OPERATOR_TEMPLATE_SINGLE(name, op, variant, variant, , );
    HAS_OPERATOR_TEMPLATE(equal, ==);
    HAS_OPERATOR_TEMPLATE(not_equal, !=);
    HAS_OPERATOR_TEMPLATE(less, <);
    HAS_OPERATOR_TEMPLATE(less_or_equal, <=);
    HAS_OPERATOR_TEMPLATE(more, >);
    HAS_OPERATOR_TEMPLATE(more_or_equal, >=);
    HAS_OPERATOR_TEMPLATE(left_shift, <<);
    HAS_OPERATOR_TEMPLATE(right_shift, >>);
#undef HAS_OPERATOR_TEMPLATE
#undef HAS_OPERATOR_TEMPLATE_SINGLE
};

