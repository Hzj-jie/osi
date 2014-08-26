
#pragma once

#ifdef HAS_TYPEDEF
    HAS_TYPEDEF redefined
#endif
#define HAS_TYPEDEF(typedef_name) \
        template <typename T> \
        stuct has_typedef_##typedef_name { \
            typedef char yes[1]; \
            typedef char no[2]; \
            template <typename U> \
            struct type_check; \
            template <typename U> \
            static yes& test(type_check<typename U::typedef_name>*); \
            template <typename> \
            static no& test(...); \
            static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes)); }

#ifdef HAS_MEMBER_FUNCTION
    HAS_MEMBER_FUNCTION redefined
#endif
#define HAS_MEMBER_FUNCTION(func) \
        template <typename T, typename RET_T, typename... Args> \
        struct has_member_function_##func { \
            typedef RET_T (T::*Sign)(Args&&...); \
            typedef char yes[1]; \
            typedef char no[2]; \
            template <typename U, U> \
            struct type_check; \
            template <typename U> \
            static yes& test(type_check<Sign, &U::func>*); \
            template <typename> \
            static no& test(...); \
            static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes)); }

#ifdef HAS_CONST_MEMBER_FUNCTION
    HAS_CONST_MEMBER_FUNCTION redefined
#endif
#define HAS_CONST_MEMBER_FUNCTION(func) \
        template <typename T, typename RET_T, typename... Args> \
        struct has_const_member_function_##func { \
            typedef RET_T (T::*Sign)(Args&&...) const; \
            typedef char yes[1]; \
            typedef char no[2]; \
            template <typename U, U> \
            struct type_check; \
            template <typename U> \
            static yes& test(type_check<Sign, &U::func>*); \
            template <typename> \
            static no& test(...); \
            static const bool value = (sizeof(test<T>(nullptr)) == sizeof(yes)); }

