
#include "../../../utils/sweeper.hpp"
#include <iostream>
using namespace std;

int main()
{
    int x = 0;
    cout << x << endl;
    {
        sweeper s([&]() { x++; }, [&]() { x++; });
    }
    cout << x << endl;
}

