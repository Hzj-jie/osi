
#pragma once
#include "../../delegates/event.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <functional>
#include <boost/predef.h>

class event_test : public icase
{
public:
    bool run() override
    {
        int x = 0;
        event<int> e({ [&](int) { x += 1; },
                       [&](int i) { x += i; } });
        event<int> e2({ new std::function<void(int)>([&](int) { x += 1; }),
                        new std::function<void(int)>([&](int i) { x += i; })});
#if BOOST_COMP_MSVC
        event<> e3({[&]() { x += 1; }});
#else
        event<> e3([&]() { x += 1; });
#endif
        e.bind([&](int i) { x += (i << 1); });
        utt_assert.equal(&(e += [&](int i) { x += (i << 2); }), &e);
        e.execute(2);
        e2.execute(2);
        e3.execute();
        // x = 1 + 2 + 1 + 2 + (2 << 1) + (2 << 2) + 1
        utt_assert.equal(x, 19);
        return true;
    }

    DEFINE_CASE(event_test);
};
REGISTER_CASE(event_test);
