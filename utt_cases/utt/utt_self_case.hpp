
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
        using namespace utt;
        assert.is_true(true);
        assert.is_true(true, CODE_POSITION());
        assert.is_false(false);
        assert.is_false(false, CODE_POSITION());
        assert.equal(std::string("abc"), std::string("abc"));
        assert.equal(std::string("abc"), std::string("abc"), CODE_POSITION());
        assert.not_equal(std::string("abc"), std::string("bcd"));
        assert.not_equal(std::string("abc"), std::string("bcd"), CODE_POSITION());
        assert.equal("abc", "abc");
        assert.equal("abc", "abc", CODE_POSITION());
        assert.not_equal("abc", "bcd");
        assert.not_equal("abc", "bcd", CODE_POSITION());
        assert.equal(1, 1);
        assert.equal(1, 1, CODE_POSITION());
        assert.not_equal(0, 1);
        assert.not_equal(0, 1, CODE_POSITION());
        return true;
    }
    DEFINE_CASE(utt_self_case);
};

REGISTER_CASE(utt_self_case);

