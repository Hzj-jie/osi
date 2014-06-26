
#include <atomic>

class single_entry
{
private:
    const int free = 0;
    const int inuse = 1;
    atomic_int v;

public:
    bool operator bool() const
    {
    }

    bool in_use() const
    {
    }

    bool not_in_use() const
    {
    }

    bool mark_in_use()
    {
    }

    void release()
    {
    }

    void mark_not_in_use()
    {
    }
};

