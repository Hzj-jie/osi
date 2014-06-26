
#include <stdint.h>
#include <thread>
using namespace std;

class processor_t
{
private:
    uint32_t _count;
    bool _single;
public:
    processor_t()
    {
        _count = thread::hardware_concurrency();
        _count = (_count == 0 ? 1 : _processor_count);
        _single = (_count == 1);
    }

    uint32_t count() const
    {
        return _count;
    }

    bool single() const
    {
        return _single;
    }
} processor;

