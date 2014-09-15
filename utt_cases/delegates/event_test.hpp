
#pragma once
#include "../../delegates/event.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <functional>

class event_test : public icase
{
public:
    bool run() override
    {
        int x = 0;
        event<int> e({ std::function<void(int)>([&](int) { x += 1; }),
                       std::function<void(int)>([&](int i) { x += i; }) });
        event<int> e2({ new std::function<void(int)>([&](int) { x += 1; }),
                        new std::function<void(int)>([&](int i) { x += i; })});
        e.bind([&](int i) { x += (i << 1); });
        utt_assert.equal(&(e += [&](int i) { x += (i << 2); }), &e);
        e.execute(2);
        e2.execute(2);
        // x = 1 + 2 + 1 + 2 + (2 << 1) + (2 << 2)
        utt_assert.equal(x, 18);
        return true;
    }

    DEFINE_CASE(event_test);
};
REGISTER_CASE(event_test);
