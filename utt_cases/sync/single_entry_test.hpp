
#pragma once
#include "../../utt/icase.hpp"
#include "../../sync/single_entry.hpp"
#include "../../utt/utt_assert.hpp"
#include <stdint.h>
#include <vector>
#include <thread>
#include "../../sync/yield.hpp"

class single_entry_test : public icase
{
private:
    const static uint32_t round_per_thread = 1000;
    single_entry se;
    int v;

    void run_case()
    {
        for(uint32_t i = 0; i < round_per_thread; i++)
        {
            while(!se.mark_in_use()) std::this_thread::force_yield();
            v++;
            se.release();
        }
    }
public:
    virtual bool execute()
    {
        using namespace std;
        v = 0;
        vector<thread> threads;
        for(uint32_t i = 0; i < processor.count - 1; i++)
            threads.push_back(thread(&single_entry_test::run_case, this));
        run_case();
        for(uint32_t i = 0; i < threads.size(); i++)
            threads[i].join();
        return true;
    }

    virtual bool cleanup()
    {
        utt_assert.equal(v, processor.count * round_per_thread, CODE_POSITION());
        return true;
    }

    virtual uint32_t preserved_processor_count() const
    {
        return processor.count;
    }

    DEFINE_CASE(single_entry_test);
};

REGISTER_CASE(single_entry_test);

