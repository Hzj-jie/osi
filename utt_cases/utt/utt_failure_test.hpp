
#pragma once
#include "../../utt/icase.hpp"

class utt_failure_test : public icase
{
public:
    bool execute() override
    {
        return utt_assert.is_true(false);
    }

    DEFINE_CASE(utt_failure_test);
};
