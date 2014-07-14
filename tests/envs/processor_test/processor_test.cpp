
#include <iostream>
#include "../../../envs/processor.hpp"
using namespace std;

int main()
{
#define p(x) cout << #x << '\t' << processor.x << endl;
    p(count);
    p(single);
#undef p
}

