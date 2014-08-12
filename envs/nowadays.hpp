
#pragma once
#include <stdint.h>
#include <chrono>
#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../const/character.hpp"
#include "../template/singleton.hpp"
#include "../utils/strutils.hpp"

static const class nowadays_t
{
private:
    nowadays_t() { }
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
        high_res_t() { }
    }& high_res = high_res_t::instance();

    const class low_res_t : public retriver<std::chrono::steady_clock>
    {
        CONST_SINGLETON(low_res_t);
    private:
        low_res_t() { }
    }& low_res = low_res_t::instance();

    const class sys_res_t : public retriver<std::chrono::system_clock>
    {
        CONST_SINGLETON(sys_res_t);
    private:
        sys_res_t() { }
    }& sys_res = sys_res_t::instance();
    CONST_SINGLETON(nowadays_t);

    const std::string long_time(const boost::posix_time::ptime& t,
                                const std::string& date_time_separator = character.blank_s(),
                                const std::string& date_separator = character.minus_sign_s(),
                                const std::string& time_separator = character.colon_s()) const
    {
        using namespace std;
        ostringstream o;
        o << (short)(t.date().year())
          << date_separator;
        o.fill('0');
        o.width(2);
        o << (short)(t.date().month())
          << date_separator;
        o.fill('0');
        o.width(2);
        o << (short)(t.date().day())
          << date_time_separator;
        o.fill('0');
        o.width(2);
        o << t.time_of_day().hours()
          << time_separator;
        o.fill('0');
        o.width(2);
        o << t.time_of_day().minutes()
          << time_separator;
        o.fill('0');
        o.width(2);
        o << t.time_of_day().seconds();
        return o.str();
    }

    const std::string long_time(const std::string& date_time_separator = character.blank_s(),
                                const std::string& date_separator = character.minus_sign_s(),
                                const std::string& time_separator = character.colon_s()) const
    {
        using namespace boost::posix_time;
        return long_time(second_clock::local_time(),
                         date_time_separator,
                         date_separator,
                         time_separator);
    }

    const std::string short_time(const boost::posix_time::ptime& t,
                                 const std::string& date_time_separator = character.blank_s(),
                                 const std::string& date_separator = character.minus_sign_s(),
                                 const std::string& time_separator = character.colon_s()) const
    {
        using namespace std;
        ostringstream o;
        o << (short)(t.date().year())
          << date_separator;
        o.fill('0');
        o.width(2);
        o << (short)(t.date().month())
          << date_separator;
        o.fill('0');
        o.width(2);
        o << (short)(t.date().day())
          << date_time_separator;
        o.fill('0');
        o.width(2);
        o << t.time_of_day().hours()
          << time_separator;
        o.fill('0');
        o.width(2);
        o << t.time_of_day().minutes()
          << time_separator;
        o.fill('0');
        o.width(2);
        o << t.time_of_day().seconds();
        return o.str();
    }

    const std::string short_time(const std::string& date_time_separator = character.blank_s(),
                                 const std::string& date_separator = character.minus_sign_s(),
                                 const std::string& time_separator = character.colon_s()) const
    {
        using namespace boost::posix_time;
        return short_time(second_clock::local_time(),
                          date_time_separator,
                          date_separator,
                          time_separator);
    }

    const std::string short_time(const boost::posix_time::ptime& t,
                                 char date_time_separator,
                                 char date_separator,
                                 char time_separator) const
    {
        return short_time(t,
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

