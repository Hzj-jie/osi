#pragma once
#include "../../primitive/data_block.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"

namespace primitive_test
{
    class data_block_int_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_block b_int = data_block::from_int32(12345);
            utt_assert.equal(b_int.as_int32(), 12345);
            utt_assert.equal(b_int.as_int64(), static_cast<int64_t>(12345));
            return true;
        }

        DEFINE_CASE(data_block_int_test);
    };

    class data_block_long_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_block b_long = data_block::from_int64(9876543210LL);
            utt_assert.equal(b_long.as_int64(), 9876543210LL);
            return true;
        }

        DEFINE_CASE(data_block_long_test);
    };

    class data_block_bool_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_block b_bool_t = data_block::from_bool(true);
            utt_assert.is_true(b_bool_t.as_bool());

            data_block b_bool_f = data_block::from_bool(false);
            utt_assert.is_false(b_bool_f.as_bool());
            return true;
        }

        DEFINE_CASE(data_block_bool_test);
    };

    class data_block_string_test : public icase
    {
    public:
        bool run() override
        {
            using namespace primitive;
            data_block b_str = data_block::from_string("hello_primitive");
            utt_assert.equal(b_str.as_string(), std::string("hello_primitive"));
            return true;
        }

        DEFINE_CASE(data_block_string_test);
    };
    REGISTER_CASE(data_block_int_test);
    REGISTER_CASE(data_block_long_test);
    REGISTER_CASE(data_block_bool_test);
    REGISTER_CASE(data_block_string_test);
}
