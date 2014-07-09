
#include "../../../utils/sweeper.hpp"
#include <iostream>
using namespace std;

static void func()
{
    static const sweeper s([]() { cout << "begin" << endl;},
                           []() { cout << "end" << endl;});
}

int main()
{
    func();
}

