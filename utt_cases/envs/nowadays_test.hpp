#pragma once
#include "../../envs/nowadays.hpp"
#include "../../utt/icase.hpp"

class nowadays_test : public icase
{
public:
    bool run() override
    {
        int64_t h_ns1 = nowadays.high_res.nanoseconds();
        int64_t h_ms1 = nowadays.high_res.milliseconds();
        int64_t l_ns1 = nowadays.low_res.nanoseconds();
        int64_t l_ms1 = nowadays.low_res.milliseconds();
        int64_t s_ns1 = nowadays.sys_res.nanoseconds();
        int64_t s_ms1 = nowadays.sys_res.milliseconds();

        if (!utt_assert.more(h_ns1, int64_t(0))) return false;
        if (!utt_assert.more(h_ms1, int64_t(0))) return false;
        if (!utt_assert.more(l_ns1, int64_t(0))) return false;
        if (!utt_assert.more(l_ms1, int64_t(0))) return false;
        if (!utt_assert.more(s_ns1, int64_t(0))) return false;
        if (!utt_assert.more(s_ms1, int64_t(0))) return false;

        std::string st = nowadays.short_time();
        if (!utt_assert.is_false(st.empty())) return false;

        std::string lt = nowadays.long_time();
        if (!utt_assert.is_false(lt.empty())) return false;

        std::string custom = nowadays.short_time('-', '-', ':');
        if (!utt_assert.is_false(custom.empty())) return false;

        int64_t h_ns2 = nowadays.high_res.nanoseconds();
        if (!utt_assert.more_or_equal(h_ns2, h_ns1)) return false;

        return true;
    }

    DEFINE_CASE(nowadays_test);
};

REGISTER_CASE(nowadays_test);
