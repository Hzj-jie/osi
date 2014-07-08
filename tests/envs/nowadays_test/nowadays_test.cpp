
#include <iostream>
#include <thread>
#include "../../../envs/nowadays.hpp"
using namespace std;
using namespace std::chrono;

int main()
{
    while(1)
    {
#define p(x) cout << #x << " ->\t" << nowadays.x.nanoseconds() << "ns\t" << nowadays.x.milliseconds() << "ms" << endl;
        p(high_res);
        p(low_res);
        p(sys_res);
#undef p
        cout << nowadays.short_time()
             << '\t'
             << nowadays.short_time(character.minus_sign, character.minus_sign, character.minus_sign)
             << endl;
        this_thread::sleep_for(seconds(1));
    }
}

