
#pragma once
#include <utility>
#include <stdint.h>
#include "../icase.hpp"

template <typename T>
class case_wrapper : public icase
{
private:
    const T c;

protected:
    bool prepare() override
    {
        return c.prepare();
    }

    bool execute() override
    {
        return c.execute();
    }

    bool cleanup() override
    {
        return c.cleanup();
    }

    bool run() final
    {
        return icase::run();
    }

public:
    template <typename... Args>
    case_wrapper(Args&&... args) : c(std::forward<Args>(args)...) { }

    virtual uint32_t preserved_processor_count() const
    {
        return c.preserved_processor_count();
    }
};

