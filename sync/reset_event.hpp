
#include <thread>
#include <atomic>
using namespace std;

class reset_event
{
private:
    const int PASS = 0;
    const int BLOCK = 1;
    bool _auto_reset;
    atomic_int _status;
public:
    bool set()
    {
        return _status.compare_exchange_weak(BLOCK, PASS);
    }

    bool operator bool() const
    {
        return (_status.load() == PASS);
    }
};

