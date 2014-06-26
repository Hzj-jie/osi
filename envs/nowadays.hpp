
#include <stdint.h>
#include <chrono>
using namespace std::chrono;

class nowadays_t
{
private:
    template<typename T>
    class retriver
    {
    public:
        int64_t nanoseconds() const
        {
            return duration_cast<duration<int64_t, nano> >(T::now()).count();
        }

        int64_t milliseconds() const
        {
            return duration_cast<duration<int64_t, milli> >(T::now()).count();
        }
    };
public:
    class high_res_t : public retriver<high_resolution_clock>
    { } high_res;

    class low_res_t : public retriver<steady_clock>
    { } low_res;

    class sys_res_t : public retriver<system_clock>
    { } sys_res;
} nowadays;

