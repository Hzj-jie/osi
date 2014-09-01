
#pragma once
#include "../../app_info/assert.hpp"
#include "../../formation/ternary.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

class ternary_test : public icase
{
private:
    const int _unknown = 0;
    const int _true = 1;
    const int _false = 2;
    bool run_case(const ternary& td, int v)
    {
        utt_assert.equal(td.true_(), v == _true);
        utt_assert.equal(td.nottrue(), v != _true);
        utt_assert.equal(td.false_(), v == _false);
        utt_assert.equal(td.notfalse(), v != _false);
        utt_assert.equal(td.unknown_(), v == _unknown);
        utt_assert.equal(td.notunknown(), v != _unknown);
        if(v == _unknown)
        {
            utt_assert.equal(td, ternary::_unknown());
            utt_assert.not_equal(td, ternary::_true());
            utt_assert.not_equal(td, ternary::_false());
        }
        else if(v == _true)
        {
            utt_assert.not_equal(td, ternary::_unknown());
            utt_assert.equal(td, ternary::_true());
            utt_assert.not_equal(td, ternary::_false());
        }
        else
        {
            assert(v == _false);
            utt_assert.not_equal(td, ternary::_unknown());
            utt_assert.not_equal(td, ternary::_true());
            utt_assert.equal(td, ternary::_false());
        }
        return true;
    }

public:
    bool run() override
    {
        // ternary();
        {
            ternary td;
            if(!run_case(td, _unknown)) return false;
        }
        // operator=(const ternary&);
        {
            ternary td = ternary::_unknown();
            if(!run_case(td, _unknown)) return false;
        }
        {
            ternary td = ternary::_true();
            if(!run_case(td, _true)) return false;
        }
        {
            ternary td = ternary::_false();
            if(!run_case(td, _false)) return false;
        }
        // operator=(bool);
        {
            ternary td = true;
            if(!run_case(td, _true)) return false;
        }
        {
            ternary td = false;
            if(!run_case(td, _false)) return false;
        }
        // ternary(const ternary&);
        {
            ternary td(ternary::_unknown());
            if(!run_case(td, _unknown)) return false;
        }
        {
            ternary td(ternary::_true());
            if(!run_case(td, _true)) return false;
        }
        {
            ternary td(ternary::_false());
            if(!run_case(td, _false)) return false;
        }
        // ternary(bool);
        {
            ternary td(true);
            if(!run_case(td, _true)) return false;
        }
        {
            ternary td(false);
            if(!run_case(td, _false)) return false;
        }
        return true;
    }

    DEFINE_CASE(ternary_test);
};
REGISTER_CASE(ternary_test);

