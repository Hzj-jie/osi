
#pragma once
#include <utility>
#include "../config.hpp"
#include "case_wrapper.hpp"

template <typename T>
class specific_case_wrapper : public case_wrapper<T>
{
public:
#define SCW_OVERRIDE(name) \
        bool name() override final { \
            if(config.specific(*this)) return case_wrapper<T>::name(); \
            else return true; }
    SCW_OVERRIDE(prepare);
    SCW_OVERRIDE(execute);
    SCW_OVERRIDE(cleanup);
#undef SCW_OVERRIDE

    template <typename... Args>
    specific_case_wrapper(Args&&... args) :
        case_wrapper<T>(std::forward<Args>(args)...) { }

    uint32_t preserved_processor_count() const override final
    {
        return case_wrapper<T>::preserved_processor_count();
    }

    const std::string& name() const override
    {
        static std::string i = std::string("specific_") + case_wrapper<T>::name();
        return i;
    }
};

