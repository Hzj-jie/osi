
#pragma once
#include <random>
#include <limits>
#include "../template/singleton.hpp"
#include <boost/predef.h>

#undef min
#undef max

class uniform_int_generator_t
{
private:
    std::random_device rd;
    std::default_random_engine eng;
    std::uniform_int_distribution<> dist;
    uniform_int_generator_t() :
        eng(rd()),
        dist(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()) { }

public:
    int min() const { return dist.min(); }
    int max() const { return dist.max(); }
    int operator()() { return dist(eng); }
    void reset() { dist.reset(); }

#if BOOST_COMP_MSVC
    static uniform_int_generator_t instance()
    {
        return uniform_int_generator_t();
    }
#else
    static uniform_int_generator_t& instance()
    {
        thread_local static uniform_int_generator_t i;
        return i;
    }
#endif
};

static int rnd_int()
{
    return uniform_int_generator_t::instance()();
}

static unsigned int rnd_uint()
{
    return uniform_int_generator_t::instance()();
}

static bool rnd_bool()
{
    return (uniform_int_generator_t::instance()() & 1) == 1;
}

// return an int in [min, max)
static int rnd_int(int min, int max)
{
    if(min >= max) return min;
    else return rnd_int() % (max - min) + min;
}

