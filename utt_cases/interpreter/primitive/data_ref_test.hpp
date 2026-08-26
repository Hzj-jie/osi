#pragma once
#include "../../../interpreter/primitive/data_ref.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"

namespace primitive_test
{
    class data_ref_abs_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_ref r = data_ref::abs(42);
            utt_assert.equal(static_cast<uint8_t>(r.type), static_cast<uint8_t>(ref_type::abs));
            utt_assert.equal(r.offset, 42);

            int64_t encoded = r.encode();
            data_ref parsed;
            utt_assert.is_true(data_ref::parse(encoded, parsed));
            utt_assert.equal(parsed, r);
            return true;
        }

        DEFINE_CASE(data_ref_abs_test);
    };

    class data_ref_rel_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_ref r = data_ref::rel(-10);
            utt_assert.equal(static_cast<uint8_t>(r.type), static_cast<uint8_t>(ref_type::rel));
            utt_assert.equal(r.offset, -10);

            int64_t encoded = r.encode();
            data_ref parsed;
            utt_assert.is_true(data_ref::parse(encoded, parsed));
            utt_assert.equal(parsed, r);
            return true;
        }

        DEFINE_CASE(data_ref_rel_test);
    };

    class data_ref_habs_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_ref r = data_ref::habs(100);
            utt_assert.equal(static_cast<uint8_t>(r.type), static_cast<uint8_t>(ref_type::habs));
            utt_assert.equal(r.offset, 100);

            int64_t encoded = r.encode();
            data_ref parsed;
            utt_assert.is_true(data_ref::parse(encoded, parsed));
            utt_assert.equal(parsed, r);
            return true;
        }

        DEFINE_CASE(data_ref_habs_test);
    };

    class data_ref_hrel_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_ref r = data_ref::hrel(-5);
            utt_assert.equal(static_cast<uint8_t>(r.type), static_cast<uint8_t>(ref_type::hrel));
            utt_assert.equal(r.offset, -5);

            int64_t encoded = r.encode();
            data_ref parsed;
            utt_assert.is_true(data_ref::parse(encoded, parsed));
            utt_assert.equal(parsed, r);
            return true;
        }

        DEFINE_CASE(data_ref_hrel_test);
    };
    REGISTER_CASE(data_ref_abs_test);
    REGISTER_CASE(data_ref_rel_test);
    REGISTER_CASE(data_ref_habs_test);
    REGISTER_CASE(data_ref_hrel_test);
}
