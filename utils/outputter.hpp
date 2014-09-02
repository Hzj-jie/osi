
#pragma once
#include <iostream>
#include <utility>
#include "../template/type_traits.hpp"
#include <type_traits>
#include "../utils/strutils.hpp"

template <typename T> struct outputter;

template <typename T>
static std::ostream& operator<<(std::ostream&, const outputter<T>&);

template <typename T>
struct outputter
{
private:
    const T x;

public:
    template <typename U>
    outputter(U&& x) :
        x(std::forward<U>(x)) { }

private:
    template <typename U>
    struct show
    {
    public:
        template <typename X>
        static std::string to_str(X&& i)
        {
            return strcat(std::forward<X>(i));
        }

        static std::ostream& output(std::ostream& os, const U& i)
        {
            os << i;
            return os;
        }

        static bool visible()
        {
            return true;
        }
    };

    template <typename U>
    struct hide
    {
    public:
        static std::string to_str()
        {
            return std::string("[not_visible]");
        }

        static std::string to_str(const U&)
        {
            return to_str();
        }

        static std::ostream& output(std::ostream& os, const U&)
        {
            os << to_str();
            return os;
        }

        static bool visible()
        {
            return false;
        }
    };

    typedef typename std::conditional<
                         operator_overloads<std::ostream, T>::
                             has_operator_left_shift_variant_const,
                         show<T>,
                         hide<T>>::type
            selected_type;

public:
    std::string to_str() const
    {
        return selected_type::to_str(x);
    }

    friend std::ostream& operator<<(std::ostream& os, const outputter<T>& i)
    {
        return selected_type::output(os, i.x);
    }

    static bool visible()
    {
        return selected_type::visible();
    }

    operator bool() const
    {
        return visible();
    }
};

template <typename T>
static outputter<typename std::remove_reference<T>::type> make_outputter(T&& i)
{
    return outputter<typename std::remove_reference<T>::type>(std::forward<T>(i));
}

