
#pragma once
#include <random>
#include <limits>
#include "../template/singleton.hpp"

#undef min
#undef max

static class uniform_int_generator_t
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

SINGLETON(uniform_int_generator_t);
}& uniform_int_generator = uniform_int_generator_t::instance();

static int rnd_int()
{
    return uniform_int_generator();
}

static unsigned int rnd_uint()
{
    return uniform_int_generator();
}

static bool rnd_bool()
{
    return (uniform_int_generator() & 1) == 1;
}

// return an int in [min, max)
static int rnd_int(int min, int max)
{
    if(min >= max) return min;
    else return rnd_int() % (max - min) + min;
}

