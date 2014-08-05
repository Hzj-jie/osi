
#pragma once
#include "../../app_info/assert.hpp"
#include "case_wrapper.hpp"
#include <utility>
#include <stdint.h>

template <typename T, uint32_t REPEAT_COUNT = 0>
class repeat_case_wrapper : public case_wrapper<T>
{
private:
    virtual uint32_t repeat_count() const
    {
        assert(REPEAT_COUNT > 0);
        return REPEAT_COUNT;
    }

public:
    bool execute() override final
    {
        for(uint32_t i = 0; i < repeat_count(); i++)
            if(!case_wrapper<T>::execute()) return false;
        return true;
    }

    template <typename... Args>
    repeat_case_wrapper(Args&&... args) :
        case_wrapper<T>(std::forward<Args>(args)...) { }
};

template <typename T, uint32_t REPEAT_COUNT = 0>
class rinne_case_wrapper : public case_forwarder<T>
{
private:
    virtual uint32_t repeat_count() const
    {
        assert(REPEAT_COUNT > 0);
        return REPEAT_COUNT;
    }

public:
    bool run() override final
    {
        for(uint32_t i = 0; i < repeat_count(); i++)
            if(!case_forwarder<T>::run()) return false;
        return true;
    }

    template <typename... Args>
    rinne_case_wrapper(Args&&... args) :
        case_forwarder<T>(std::forward<Args>(args)...) { }
};

