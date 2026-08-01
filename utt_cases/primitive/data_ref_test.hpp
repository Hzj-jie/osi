#pragma once
#include "../../primitive/data_ref.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

class data_ref_test : public icase
{
public:
    bool run() override
    {
        using namespace primitive;

        data_ref r1 = data_ref::abs(42);
        utt_assert.equal(static_cast<uint8_t>(r1.type), static_cast<uint8_t>(ref_type::abs));
        utt_assert.equal(r1.offset, 42);

        int64_t encoded = r1.encode();
        data_ref parsed;
        utt_assert.is_true(data_ref::parse(encoded, parsed));
        utt_assert.equal(parsed, r1);

        data_ref r2 = data_ref::rel(-10);
        utt_assert.equal(static_cast<uint8_t>(r2.type), static_cast<uint8_t>(ref_type::rel));
        utt_assert.equal(r2.offset, -10);

        encoded = r2.encode();
        utt_assert.is_true(data_ref::parse(encoded, parsed));
        utt_assert.equal(parsed, r2);

        data_ref r3 = data_ref::habs(100);
        utt_assert.equal(static_cast<uint8_t>(r3.type), static_cast<uint8_t>(ref_type::habs));
        utt_assert.equal(r3.offset, 100);

        encoded = r3.encode();
        utt_assert.is_true(data_ref::parse(encoded, parsed));
        utt_assert.equal(parsed, r3);

        data_ref r4 = data_ref::hrel(-5);
        utt_assert.equal(static_cast<uint8_t>(r4.type), static_cast<uint8_t>(ref_type::hrel));
        utt_assert.equal(r4.offset, -5);

        encoded = r4.encode();
        utt_assert.is_true(data_ref::parse(encoded, parsed));
        utt_assert.equal(parsed, r4);

        return true;
    }

    DEFINE_CASE(data_ref_test);
};

REGISTER_CASE(data_ref_test);
