
#include <vector>
#include "icase.hpp"
using namespace std;

static vector<icase> cases;

#define REGISTER_CASE(x) \
    class REGISTER_CASE_INSERTER_##x_t { \
    public: \
        REGISTER_CASE_INSERTER_##x() { \
            cases.push_back(x()); } } \
        REGISTER_CASE_INSERTER_##x;

