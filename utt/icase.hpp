
#pragma once
#include <string>
#include <list>
#include <mutex>

class icase
{
public:
    virtual bool run() = 0;
    virtual const std::string& name() const = 0;
    virtual ~icase() { }
};

#define DEFINE_CASE(x) \
    public: \
        virtual const std::string& name() const { \
            static const std::string i(#x); \
            return i;}

#define REGISTER_CASE(x) \
    const static class REGISTER_CASE_INSERTER_##x_t { \
        private: \
            REGISTER_CASE_INSERTER_##x_t() { \
                utt::cases.push_back(new x()); } \
        CONST_SINGLETON(REGISTER_CASE_INSERTER_##x_t); \
        }& REGISTER_CASE_INSERTER_##x = REGISTER_CASE_INSERTER_##x_t::instance() ; \

namespace utt
{
    using namespace std;
    static list<icase*> cases;
    
    static bool fetch_next_case(icase*& r)
    {
        r = nullptr;
        static mutex mtx;
        unique_lock<mutex> lck(mtx);
        if(cases.empty()) return false;
        else
        {
            r = cases.front();
            cases.pop_front();
            return true;
        }
    }

    static void finish_case(icase* r)
    {
        delete r;
    }
}

