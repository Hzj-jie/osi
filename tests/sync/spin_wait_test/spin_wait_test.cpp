
#include "../../../sync/spin_wait.hpp"

int main()
{
    std::this_thread::wait_until([]() { return false; });
    return 0;
}

