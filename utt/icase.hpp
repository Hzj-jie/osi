
#pragma once
#include <string>
#include <list>
#include <mutex>
#include "../sync/lock.hpp"
#include <stdint.h>
#include "utt_assert.hpp"
#include "../app_info/trace.hpp"
#include "../app_info/assert.hpp"

class icase
{
public:
    utt::assert_t utt_assert;

    virtual bool prepare()
    {
        return true;
    }

    virtual bool execute()
    {
        return assert(false);
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

    virtual ~icase() = default;

    virtual void set_case_name(const std::string& name)
    {
        utt_assert.set_case_name(name);
    }

    void operator()()
    {
        set_case_name(name());
        utt_assert.is_true(run(), CODE_POSITION());
    }
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
    static std::list<icase*> cases;
    static std::mutex mtx;
    
    static bool fetch_next_case(icase*& r)
    {
        r = nullptr;
        scope_lock(mtx);
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
        scope_lock(mtx);
        cases.push_back(c);
    }
}

