
#pragma once
#include <functional>

template <typename T>
class lazier
{
private:
    mutable volatile bool initialized;
    mutable T v;
    const std::function<T()> init;

public:
    lazier(const std::function<T()>& init) :
        initialized(false),
        init(init) { }
    
    lazier(std::function<T()>&& init) :
        initialized(false),
        init(init) { }

    const T& value() const
    {
        if(!initialized)
        {
            v = init();
            initialized = true;
        }
        return v;
    }
};

