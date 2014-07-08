
#include <iostream>
#include "../../../envs/exeinfo.hpp"
using namespace std;

int main()
{
#define p(x) cout << #x << '\t' << exeinfo.x() << endl;
    p(path);
    p(name);
    p(directory);
#undef p
}

