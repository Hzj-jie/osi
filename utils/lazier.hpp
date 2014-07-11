
#pragma once
#include <functional>

template <typename T>
class lazier
{
private:
    volatile bool initialized;
    T v;
    std::function<T()> init;

public:
    lazier(const std::function<T()>& init) :
        initialized(false),
        init(init) { }
    
    lazier(std::function<T()>&& init) :
        initialized(false),
        init(init) { }

    const T& value()
    {
        if(!initialized)
        {
            v = init();
            initialized = true;
        }
        return v;
    }
};

