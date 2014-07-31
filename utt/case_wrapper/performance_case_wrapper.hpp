
#pragma once
#include "case_wrapper.hpp"
#include "../../app_info/assert.hpp"
#include "../../envs/processor.hpp"
#include "../../utils/timing_counter.hpp"
#include "../utt_assert.hpp"
#include "../utt_error_handle.hpp"
#include <stdint.h>
#include <functional>

template <typename T, uint32_t MinLoops = UINT32_MIN, uint32_t MaxLoops = UINT32_MAX, uint32_t RunTimes = 1>
class performance_case_wrapper : public case_wrapper<T>
{
private:
    virtual uint32_t min_loops() const
    {
        return MinLoops;
    }

    virtual uint32_t max_loops() const
    {
        return MaxLoops;
    }

    virtual uint32_t run_times() const
    {
        return RunTimes;
    }

public:
    uint32_t preserved_processor_count() const final
    {
        return processor.count;
    }

    bool execute() override final
    {
        int64_t min = UINT32_MAX;
        assert(run_times() > 0);
        for(uint32_t i = 0; i < run_times(); i++)
        {
            int64_t used_loops;
            processor_loops_counter counter(std::ref(used_loops));
            if(!case_wrapper<T>::execute()) return false;
            if(used_loops < min) min = used_loops;
        }
        
        if(min_loops() == UINT32_MIN) utt::utt_raise_error("performance case ", name(), " has not provided min_loops");
        if(max_loops() == UINT32_MAX) utt::utt_raise_error("performance case ", name(), " has not provided max_loops");
        utt_assert.more_or_equal(min, min_loops());
        utt_assert.less_or_equal(min, max_loops());
        return true;
    }
};

