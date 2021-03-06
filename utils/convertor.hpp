
#pragma once
#include "../template/singleton.hpp"
#include <stdint.h>
#include <string>
#include "strutils.hpp"
#include <type_traits>
#include <boost/algorithm/string.hpp>
#include "../template/type_traits.hpp"
#include "../app_info/assert.hpp"

namespace converts
{
    const static char* True = "true";
    const static char* False = "false";

    static bool convert(const std::string& i, std::string& o)
    {
        o = i;
        return true;
    }

    static bool convert(const char* i, std::string& o)
    {
        o = i;
        return true;
    }

    static bool convert(std::string&& i, bool& o)
    {
        std::string s(std::move(i));
        boost::algorithm::to_lower(s);
        o = (s == True);
        return true;
    }

    static bool convert(const std::string& i, bool& o)
    {
        return convert(std::string(i), o);
    }

    static bool convert(const char* i, bool& o)
    {
        return convert(std::string(i), o);
    }

    static bool convert(const bool& i, std::string& o)
    {
        o = std::string(i ? True : False);
        return true;
    }

    template <typename T>
    static bool convert(const std::string& i, T& o)
    {
        return from_str(i, o);
    }

    template <typename T>
    static bool convert(const T& i, std::string& o)
    {
        return to_str(i, o);
    }

    template <typename T>
    static bool convert(const char* i, T& o)
    {
        return convert(std::string(i), o);
    }

    struct copy_assign
    {
        template <typename T1, typename T2>
        static bool copy(const T1& i, T2& o)
        {
            o = i;
            return true;
        }
    };

    struct copy_constructible
    {
        template <typename T1, typename T2>
        static bool copy(const T1& i, T2& o)
        {
            new (&o) T2(i);
            return true;
        }
    };

    struct copy_convert_to
    {
        template <typename T1, typename T2>
        static bool copy(const T1& i, T2& o)
        {
            return i.convert_to(o);
        }
    };

    struct copy_failure
    {
        template <typename T1, typename T2>
        static bool copy(const T1& i, T2& o)
        {
            return false;
        }
    };

    struct class_convertor
    {
    private:
        HAS_CONST_MEMBER_FUNCTION(convert_to);

    public:
        template <typename T1, typename T2>
        static bool copy(const T1& i, T2& o)
        {
            typedef typename std::conditional<
                                 has_const_member_function_convert_to<T1, bool, T2&>::value,
                                 copy_convert_to,
                                 copy_failure
                             >::type
                             copy_type;
            return copy_type::copy(i, o);
        }
    };

    template <typename T1, typename T2>
    static bool convert(const T1& i, T2& o)
    {
        typedef typename std::conditional<
                             // std::is_convertible<T1, T2>::value ||
                             std::is_assignable<T2&, T1>::value,
                             copy_assign,
                             typename std::conditional<
                                 std::is_constructible<T2, const T1&>::value,
                                 copy_constructible,
                                 typename std::conditional<
                                     std::is_class<T1>::value,
                                     class_convertor,
                                     copy_failure
                                 >::type
                             >::type
                         >::type
                         copy_type;
        return copy_type::copy(i, o);
    }
}

const static class convertor_t
{
public:
    template <typename T1, typename T2>
    bool operator()(const T1& i, T2& o) const
    {
        return converts::convert(i, o);
    }

    template <typename T1, typename T2>
    T2 convert(const T1& i) const
    {
        T2 o { };
        assert(operator()(i, o));
        return o;
    }

private:
    convertor_t() = default;

    CONST_SINGLETON(convertor_t);
}& convertor = convertor_t::instance();

