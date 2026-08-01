#pragma once
#include <stdint.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include "../const/character.hpp"
#include "../template/singleton.hpp"
#include "../utils/strutils.hpp"

static const class nowadays_t
{
private:
    nowadays_t() = default;
    template<typename T>
    class retriver
    {
    public:
        int64_t nanoseconds() const
        {
            using namespace std;
            using namespace std::chrono;
            return duration_cast<duration<int64_t, nano>>(T::now().time_since_epoch()).count();
        }

        int64_t milliseconds() const
        {
            using namespace std;
            using namespace std::chrono;
            return duration_cast<duration<int64_t, milli>>(T::now().time_since_epoch()).count();
        }
    };
public:
    const class high_res_t : public retriver<std::chrono::high_resolution_clock>
    {
        CONST_SINGLETON(high_res_t);
    private:
        high_res_t() = default;
    }& high_res = high_res_t::instance();

    const class low_res_t : public retriver<std::chrono::steady_clock>
    {
        CONST_SINGLETON(low_res_t);
    private:
        low_res_t() = default;
    }& low_res = low_res_t::instance();

    const class sys_res_t : public retriver<std::chrono::system_clock>
    {
        CONST_SINGLETON(sys_res_t);
    private:
        sys_res_t() = default;
    }& sys_res = sys_res_t::instance();
    CONST_SINGLETON(nowadays_t);

#define TIMER_TEMPLATE(x, y) \
    const class x##_res_##y##_t { \
        CONST_SINGLETON(x##_res_##y##_t); \
    public: \
        int64_t operator()() const { \
            return x##_res_t::instance().y(); } \
    private:\
        x##_res_##y##_t() { } }& x##_res_##y = x##_res_##y##_t::instance();
    TIMER_TEMPLATE(high, milliseconds);
    TIMER_TEMPLATE(high, nanoseconds);
    TIMER_TEMPLATE(low, milliseconds);
    TIMER_TEMPLATE(low, nanoseconds);
    TIMER_TEMPLATE(sys, milliseconds);
    TIMER_TEMPLATE(sys, nanoseconds);
#undef TIMER_TEMPLATE

    typedef high_res_milliseconds_t high_res_ms_t;
    typedef low_res_milliseconds_t low_res_ms_t;

    const std::string long_time(const std::chrono::system_clock::time_point& tp,
                                const std::string& date_time_separator = character.blank_s(),
                                const std::string& date_separator = character.minus_sign_s(),
                                const std::string& time_separator = character.colon_s()) const
    {
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm t;
#if defined(_WIN32)
        localtime_s(&t, &tt);
#else
        localtime_r(&tt, &t);
#endif
        std::ostringstream o;
        o << (1900 + t.tm_year) << date_separator;
        o.fill('0');
        o.width(2);
        o << (1 + t.tm_mon) << date_separator;
        o.fill('0');
        o.width(2);
        o << t.tm_mday << date_time_separator;
        o.fill('0');
        o.width(2);
        o << t.tm_hour << time_separator;
        o.fill('0');
        o.width(2);
        o << t.tm_min << time_separator;
        o.fill('0');
        o.width(2);
        o << t.tm_sec;
        return o.str();
    }

    const std::string long_time(const std::string& date_time_separator = character.blank_s(),
                                const std::string& date_separator = character.minus_sign_s(),
                                const std::string& time_separator = character.colon_s()) const
    {
        return long_time(std::chrono::system_clock::now(),
                         date_time_separator,
                         date_separator,
                         time_separator);
    }

    const std::string short_time(const std::chrono::system_clock::time_point& tp,
                                 const std::string& date_time_separator = character.blank_s(),
                                 const std::string& date_separator = character.minus_sign_s(),
                                 const std::string& time_separator = character.colon_s()) const
    {
        return long_time(tp, date_time_separator, date_separator, time_separator);
    }

    const std::string short_time(const std::string& date_time_separator = character.blank_s(),
                                 const std::string& date_separator = character.minus_sign_s(),
                                 const std::string& time_separator = character.colon_s()) const
    {
        return long_time(date_time_separator, date_separator, time_separator);
    }

    const std::string short_time(const std::chrono::system_clock::time_point& tp,
                                 char date_time_separator,
                                 char date_separator,
                                 char time_separator) const
    {
        return short_time(tp,
                          to_str(date_time_separator),
                          to_str(date_separator),
                          to_str(time_separator));
    }

    const std::string short_time(char date_time_separator,
                                 char date_separator,
                                 char time_separator) const
    {
        return short_time(to_str(date_time_separator),
                          to_str(date_separator),
                          to_str(time_separator));
    }
}& nowadays = nowadays_t::instance();
