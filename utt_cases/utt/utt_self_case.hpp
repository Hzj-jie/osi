
#pragma once
#include <string>
#include "../../app_info/trace.hpp"
#include "../../utt/cases.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

class utt_self_case : public icase
{
public:
    virtual bool run()
    {
        utt_assert.is_true(true);
        utt_assert.is_true(true, CODE_POSITION());
        utt_assert.is_false(false);
        utt_assert.is_false(false, CODE_POSITION());
        utt_assert.equal(std::string("abc"), std::string("abc"));
        utt_assert.equal(std::string("abc"), std::string("abc"), CODE_POSITION());
        utt_assert.not_equal(std::string("abc"), std::string("bcd"));
        utt_assert.not_equal(std::string("abc"), std::string("bcd"), CODE_POSITION());
        utt_assert.equal("abc", "abc");
        utt_assert.equal("abc", "abc", CODE_POSITION());
        utt_assert.not_equal("abc", "bcd");
        utt_assert.not_equal("abc", "bcd", CODE_POSITION());
        utt_assert.equal(1, 1);
        utt_assert.equal(1, 1, CODE_POSITION());
        utt_assert.not_equal(0, 1);
        utt_assert.not_equal(0, 1, CODE_POSITION());
        return true;
    }
    DEFINE_CASE(utt_self_case);
};

REGISTER_CASE(utt_self_case);

