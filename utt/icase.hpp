
#pragma once
#include <string>
#include <list>
#include <mutex>
#include "../app_info/k_assert.hpp"
#include <stdint.h>

class icase
{
public:
    virtual bool prepare()
    {
        return true;
    }

    virtual bool execute()
    {
        k_assert(false);
        return false;
    }

    virtual bool cleanup()
    {
        return true;
    }

    virtual bool run()
    {
        return prepare() &&
               execute() &&
               cleanup();
    }

    virtual uint32_t preserved_processor_count() const
    {
        return 1;
    }

    virtual const std::string& name() const = 0;

    virtual ~icase() { }
};

#define DEFINE_CASE(x) \
    public: \
        virtual const std::string& name() const { \
            static const std::string i(#x); \
            return i;}

#define REGISTER_CASE(x) \
    const static class REGISTER_CASE_INSERTER_##x##_t { \
        private: \
            REGISTER_CASE_INSERTER_##x##_t() { \
                utt::cases.push_back(new x()); } \
        CONST_SINGLETON(REGISTER_CASE_INSERTER_##x##_t); \
        }& REGISTER_CASE_INSERTER_##x = REGISTER_CASE_INSERTER_##x##_t::instance() ; \

namespace utt
{
    using namespace std;
    static list<icase*> cases;
    static mutex mtx;
    
    static bool fetch_next_case(icase*& r)
    {
        r = nullptr;
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

    static void pending_case(icase* c)
    {
        k_assert(c != nullptr);
        unique_lock<mutex> lck(mtx);
        cases.push_back(c);
    }
}

