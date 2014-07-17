
#pragma once
#include <functional>
#include "../../sync/spin_wait.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <stdint.h>

class spin_wait_test : public icase
{
private:
    static bool wait_when_test_1(const function<void(const function<bool()>&)>& f)
    {
        const int target = 100;
        int x = 0;
        f([&]()
        {
            x++;
            return x < target;
        });
        utt_assert.equal(x, target);
        return true;
    }

    static bool wait_when_test_2(const function<bool(const function<bool()>&, uint32_t)>& f)
    {
        const int target = 100;
        int x = 0;
        utt_assert.is_true(f([&]()
                           {
                               x++;
                               return x < target;
                           },
                           UINT32_MAX));
        utt_assert.equal(x, target);
        x = 0;
        utt_assert.is_false(f([&]()
                            {
                                x++;
                                return x < UINT32_MAX; 
                            },
                            1));
        utt_assert.more(x, 0);
        return true;
    }

    static bool wait_when_no_timeout_test()
    {
        return wait_when_test_1(std::this_thread::lazy_wait_when_1) &&
               wait_when_test_1(std::this_thread::wait_when_1) &&
               wait_when_test_1(std::this_thread::strict_wait_when_1);
    }

    template <typename T>
    static bool wait_when_timeout_test(T&& ms_timer)
    {
#define WAIT_WHEN_TIMER_CONVERTOR(x) \
            [&](const function<bool()>& f, uint32_t timeout_ms) { return x(ms_timer, f, timeout_ms); }
        return wait_when_test_2(WAIT_WHEN_TIMER_CONVERTOR(std::this_thread::lazy_wait_when_2)) &&
               wait_when_test_2(WAIT_WHEN_TIMER_CONVERTOR(std::this_thread::wait_when_2)) &&
               wait_when_test_2(WAIT_WHEN_TIMER_CONVERTOR(std::this_thread::strict_wait_when_2));
#undef WAIT_WHEN_TIMER_CONVERTOR
    }

    static bool wait_when_timeout_test()
    {
        return wait_when_test_2(std::this_thread::lazy_wait_when_3) &&
               wait_when_test_2(std::this_thread::wait_when_3) &&
               wait_when_test_2(std::this_thread::strict_wait_when_3);
    }

    static bool wait_when_timeout_test_2()
    {
        return wait_when_timeout_test(nowadays.low_res) &&
               wait_when_timeout_test(nowadays.high_res);
    }
public:
    virtual bool run()
    {
        return wait_when_no_timeout_test() &&
               wait_when_timeout_test() &&
               wait_when_timeout_test_2();
    }

    DEFINE_CASE(spin_wait_test);
};

REGISTER_CASE(spin_wait_test);

