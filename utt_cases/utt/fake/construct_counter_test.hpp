
#pragma once
#include "../../../utt/fake/construct_counter.hpp"
#include "../../../utt/icase.hpp"
#include "../../../utt/utt_assert.hpp"
#include <stdint.h>
#include "../../../envs/processor.hpp"
#include "../../../app_info/trace.hpp"

class construct_counter_test : public icase
{
private:
    template <typename T>
    bool run_case()
    {
        typedef construct_counter<T> cc;
        utt_assert.equal(cc::default_constructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::copy_constructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::move_constructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::destructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::instance_count(), 0, CODE_POSITION());

        {
            cc x;
            utt_assert.equal(cc::default_constructed(), 1, CODE_POSITION());
            utt_assert.equal(cc::copy_constructed(), 0, CODE_POSITION());
            utt_assert.equal(cc::move_constructed(), 0, CODE_POSITION());
            utt_assert.equal(cc::destructed(), 0, CODE_POSITION());
            utt_assert.equal(cc::instance_count(), 1, CODE_POSITION());
        }
        utt_assert.equal(cc::default_constructed(), 1, CODE_POSITION());
        utt_assert.equal(cc::copy_constructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::move_constructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::destructed(), 1, CODE_POSITION());
        utt_assert.equal(cc::instance_count(), 0, CODE_POSITION());


        {
            cc x;
            cc y(x);
            utt_assert.equal(cc::default_constructed(), 2, CODE_POSITION());
            utt_assert.equal(cc::copy_constructed(), 1, CODE_POSITION());
            utt_assert.equal(cc::move_constructed(), 0, CODE_POSITION());
            utt_assert.equal(cc::destructed(), 1, CODE_POSITION());
            utt_assert.equal(cc::instance_count(), 2, CODE_POSITION());
        }
        utt_assert.equal(cc::default_constructed(), 2, CODE_POSITION());
        utt_assert.equal(cc::copy_constructed(), 1, CODE_POSITION());
        utt_assert.equal(cc::move_constructed(), 0, CODE_POSITION());
        utt_assert.equal(cc::destructed(), 3, CODE_POSITION());
        utt_assert.equal(cc::instance_count(), 0, CODE_POSITION());

        {
            cc y(cc());
            utt_assert.equal(cc::default_constructed(), 3, CODE_POSITION());
            utt_assert.equal(cc::copy_constructed(), 1, CODE_POSITION());
            utt_assert.equal(cc::move_constructed(), 1, CODE_POSITION());
            utt_assert.equal(cc::destructed(), 4, CODE_POSITION());
            utt_assert.equal(cc::instance_count(), 1, CODE_POSITION());
        }
        utt_assert.equal(cc::default_constructed(), 3, CODE_POSITION());
        utt_assert.equal(cc::copy_constructed(), 1, CODE_POSITION());
        utt_assert.equal(cc::move_constructed(), 1, CODE_POSITION());
        utt_assert.equal(cc::destructed(), 5, CODE_POSITION());
        utt_assert.equal(cc::instance_count(), 0, CODE_POSITION());

        return true;
    }

public:
    uint32_t preserved_processor_count() const
    {
        return processor.count;
    }

    bool prepare() override
    {
        construct_counter<int>::reset();
        construct_counter<unsigned int>::reset();
        return icase::prepare();
    }

    bool execute() override
    {
        return run_case<int>() &&
               run_case<unsigned int>();
    }

    bool cleanup() override
    {
        construct_counter<int>::reset();
        construct_counter<unsigned int>::reset();
        return icase::cleanup();
    }

    DEFINE_CASE(construct_counter_test);
};
REGISTER_CASE(construct_counter_test);

