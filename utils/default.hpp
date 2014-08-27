
#if 0
#pragma once
#include <type_traits>
#include <utility>

namespace __make_default_private
{
    template <typename T>
    struct no_parameter_constructor
    {
        static T&& make_default()
        {
            return std::move(T());
        }
    };

    template <typename T>
    struct declval_constructor
    {
        static T&& make_default()
        {
            return std::move(std::declval<T>());
        }
    };
}

template <typename T>
static T&& make_default()
{
    using namespace __make_default_private;
    typedef typename std::conditional<
                         std::is_constructible<T>::value,
                         no_parameter_constructor<T>,
                         declval_constructor<T>>::type
                     cons_type;
    return std::move(cons_type::make_default());
}
#endif

#if 0 // BUGBUG IN G++ ?
#pragma once
#include <type_traits>
#include <utility>

namespace __make_default_private
{
    struct no_parameter_constructor
    {
        template <typename T>
        static T&& make_default()
        {
            return std::move(T());
        }
    };

    struct declval_constructor
    {
        template <typename T>
        static T&& make_default()
        {
            return std::move(std::declval<T>());
        }
    };
}

template <typename T>
static T&& make_default()
{
    using namespace __make_default_private;
    typedef typename std::conditional<
                         std::is_constructible<T>::value,
                         no_parameter_constructor,
                         declval_constructor>::type
                     cons_type;
    return std::move(cons_type::make_default<T>());
}
#endif

