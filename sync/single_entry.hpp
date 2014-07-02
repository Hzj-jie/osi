
#include <atomic>

class single_entry
{
private:
    const int free = 0;
    const int inuse = 1;
    std::atomic_int v;

public:
    single_entry() : single_entry(false) { }
    single_entry(bool init_value) :
        v(init_value ? inuse : free) { }

    bool operator bool() const
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
        return v.compare_exchange_weak(free, inuse);
    }

    void release()
    {
        // TODO: wait for assert
        mark_not_in_use();
    }

    void mark_not_in_use()
    {
        v.store(free);
    }
};

