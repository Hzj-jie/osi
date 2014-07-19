
#pragma once
#include <utility>
#include <stdint.h>
#include "case_wrapper.hpp"
#include <thread>
#include <vector>

template <typename T>
class multithreading_case_wrapper : public case_wrapper
{
private:
    const uint32_t tc;

    virtual void execute(uint32_t id)
    {
        return case_wrapper::execute();
    }

    bool execute() override final
    {
        using namespace std;
        vector<thread> vs;
        for(uint32_t i = 0; i < thread_count() - 1; i++)
            vs.push_back(thread(&multithreading_case_wrapper::execute, this, i));
        execute(thread_count() - 1);
        for(uint32_t i = 0; i < vs.size(); i++)
            vs[i].join();
        return true;
    }

    virtual uint32_t thread_count() const
    {
        assert(tc > 0);
        return tc;
    }

public:
    template <typename... Args>
    multithreading_case_wrapper(uint32_t thread_count,
                                Args&&... args) :
        case_wrapper(std::forward<Args>(args)...),
        tc(thread_count) { }

    template <typename... Args>
    multithreading_case_wrapper(Args&&... args) :
        multithreading_case_wrapper(0, std::forward<Args>(args)...) { }

    uint32_t preserved_processor_count() const override final
    {
        return thread_count();
    }
};

