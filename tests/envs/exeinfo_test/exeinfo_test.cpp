
#include <iostream>
#include "../../../envs/exeinfo.hpp"
using namespace std;

int main()
{
#define p(x) cout << #x << '\t' << exeinfo.x() << endl;
    p(path);
#undef p
}

