
#include <stdint.h>
#include <chrono>
#include "../template/singleton.hpp"

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
            return duration_cast<duration<int64_t, nano> >(T::now().time_since_epoch()).count();
        }

        int64_t milliseconds() const
        {
            using namespace std;
            using namespace std::chrono;
            return duration_cast<duration<int64_t, milli> >(T::now().time_since_epoch()).count();
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
}& nowadays = nowadays_t::instance();

