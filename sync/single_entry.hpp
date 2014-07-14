
#pragma once
#include <atomic>
#include "../app_info/trace.hpp"
#include "../app_info/assert.hpp"

class single_entry
{
private:
    // the compare_exchange_weak value is T instead of const T&
    const int free = 0;
    const int inuse = 1;
    // BUG IN VC 18, the atomic_int does not have constructor with parameter
    std::atomic<int> v;

public:
    single_entry() : single_entry(false) { }
    single_entry(bool init_value) :
        v(init_value ? inuse : free) { }

    operator bool()
    {
        return in_use();
    }

    bool in_use() const
    {
        return (v == inuse);
    }

    bool not_in_use() const
    {
        return !in_use();
    }

    bool mark_in_use()
    {
        int f = free;
        return v.compare_exchange_weak(f, inuse) &&
		       assert(f == free);
    }

    void release()
    {
        assert(in_use(), CODE_POSITION());
        mark_not_in_use();
    }

    void mark_not_in_use()
    {
        v.store(free);
    }
};

