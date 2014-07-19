
#pragma once
#include "../../app_info/assert.hpp"
#include <utility>
#include <stdint.h>
#include "case_wrapper.hpp"
#include <thread>
#include <vector>

template <typename T, uint32_t THREAD_COUNT = 0>
class multithreading_case_wrapper : public case_wrapper<T>
{
private:
    virtual void execute(uint32_t id)
    {
        case_wrapper<T>::execute();
    }

    inline void thread_execute(uint32_t id) { execute(id); }

    virtual uint32_t thread_count() const
    {
        assert(THREAD_COUNT > 0);
        return THREAD_COUNT;
    }

public:
    bool execute() override final
    {
        using namespace std;
        vector<thread> vs;
        for(uint32_t i = 0; i < thread_count() - 1; i++)
            vs.push_back(thread(&multithreading_case_wrapper::thread_execute, this, i));
        execute(thread_count() - 1);
        for(uint32_t i = 0; i < vs.size(); i++)
            vs[i].join();
        return true;
    }

    template <typename... Args>
    multithreading_case_wrapper(Args&&... args) :
        case_wrapper<T>(std::forward<Args>(args)...) { }

    uint32_t preserved_processor_count() const override final
    {
        return thread_count();
    }
};

