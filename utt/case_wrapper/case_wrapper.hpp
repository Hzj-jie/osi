
#pragma once
#include <utility>
#include <stdint.h>
#include "../icase.hpp"
#include <string>

template <typename T>
class case_forwarder : public icase
{
private:
    T c;

public:
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

    bool run() override
    {
        return c.run();
    }

    template <typename... Args>
    case_forwarder(Args&&... args) : c(std::forward<Args>(args)...) { }

    uint32_t preserved_processor_count() const override
    {
        return c.preserved_processor_count();
    }

    const std::string& name() const override
    {
        return c.name();
    }

    void set_case_name(const std::string& name) override final
    {
        icase::set_case_name(name);
        c.set_case_name(name);
    }
};

template <typename T>
class case_wrapper : public case_forwarder<T>
{
public:
    bool run() override final
    {
        return icase::run();
    }

    template <typename... Args>
    case_wrapper(Args&&... args) :
        case_forwarder<T>(std::forward<Args>(args)...) { }
};

