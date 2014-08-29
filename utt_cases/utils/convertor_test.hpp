
#pragma once
#include "../../utils/convertor.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include <string>
#include <stdint.h>
#include "../../app_info/trace.hpp"

class convertor_test : public icase
{
private:
    class int_constructable_class
    {
    public:
        const int v;

        int_constructable_class(int i) :
            v(i) { }
        
        int_constructable_class() :
            int_constructable_class(0) { }

        bool convert_to(int& o) const
        {
            o = v;
            return true;
        }
    };

    class int_assignable_class
    {
    private:
        int v;

    public:
        int_assignable_class(int i) :
            v(i) { }

        int_assignable_class() :
            int_assignable_class(0) { }

        int_assignable_class& operator=(const int& i)
        {
            v = i;
            return *this;
        }

        int operator*() const
        {
            return v;
        }

        bool convert_to(int& o) const
        {
            o = v;
            return true;
        }
    };

    class no_int_class
    {
    private:
        int v;

    public:
        no_int_class() = default;

        void set(int x)
        {
            v = x;
        }

        int get() const
        {
            return v;
        }
    };

    bool str_int_case() const
    {
        int i = 0;
        std::string s;

        utt_assert.is_true(convertor("100", i), CODE_POSITION());
        utt_assert.equal(i, int(100), CODE_POSITION());

        utt_assert.is_true(convertor("1ab", i), CODE_POSITION());
        utt_assert.equal(i, int(1), CODE_POSITION());

        utt_assert.is_false(convertor("abc", i), CODE_POSITION());

        utt_assert.is_true(convertor("200", i), CODE_POSITION());
        utt_assert.equal(i, int(200), CODE_POSITION());

        utt_assert.is_true(convertor(int(100), s), CODE_POSITION());
        utt_assert.equal(s, std::string("100"), CODE_POSITION());

        utt_assert.is_true(convertor(int(200), s), CODE_POSITION());
        utt_assert.equal(s, std::string("200"), CODE_POSITION());

        return true;
    }

    bool str_bool_case() const
    {
        bool b = false;
        std::string s;

        utt_assert.is_true(convertor("True", b), CODE_POSITION());
        utt_assert.equal(b, true, CODE_POSITION());

        utt_assert.is_true(convertor("FaLse", b), CODE_POSITION());
        utt_assert.equal(b, false, CODE_POSITION());

        utt_assert.is_true(convertor("AbCDEfg", b), CODE_POSITION());
        utt_assert.equal(b, false, CODE_POSITION());

        utt_assert.is_true(convertor(true, s), CODE_POSITION());
        utt_assert.equal(s, std::string("true"), CODE_POSITION());

        utt_assert.is_true(convertor(false, s), CODE_POSITION());
        utt_assert.equal(s, std::string("false"), CODE_POSITION());

        return true;
    }

    bool int_uint32_case() const
    {
        int i = 0;
        uint32_t u = 0;

        utt_assert.is_true(convertor(int(100), u), CODE_POSITION());
        utt_assert.equal(u, uint32_t(100), CODE_POSITION());

        utt_assert.is_true(convertor(int(200), u), CODE_POSITION());
        utt_assert.equal(u, uint32_t(200), CODE_POSITION());

        utt_assert.is_true(convertor(uint32_t(100), i), CODE_POSITION());
        utt_assert.equal(i, int(100), CODE_POSITION());

        utt_assert.is_true(convertor(uint32_t(200), i), CODE_POSITION());
        utt_assert.equal(i, int(200), CODE_POSITION());

        utt_assert.is_true(convertor(INT_MIN, u), CODE_POSITION());
        utt_assert.equal(u, uint32_t(INT_MIN), CODE_POSITION());

        utt_assert.is_true(convertor(UINT32_MAX, i), CODE_POSITION());
        utt_assert.equal(i, int(UINT32_MAX), CODE_POSITION());

        return true;
    }

    bool int_constructable_class_case() const
    {
        typedef int_constructable_class C;
        C c;
        int i = 0;

        utt_assert.is_true(convertor(C(100), i), CODE_POSITION());
        utt_assert.equal(i, int(100), CODE_POSITION());

        utt_assert.is_true(convertor(int(100), c), CODE_POSITION());
        utt_assert.equal(c.v, int(100), CODE_POSITION());

        return true;
    }

    bool int_assignable_class_case() const
    {
        typedef int_assignable_class C;
        C c;
        int i = 0;

        utt_assert.is_true(convertor(C(100), i), CODE_POSITION());
        utt_assert.equal(i, int(100), CODE_POSITION());

        utt_assert.is_true(convertor(int(100), c), CODE_POSITION());
        utt_assert.equal(*c, int(100), CODE_POSITION());

        return true;
    }

    bool no_int_class_case() const
    {
        typedef no_int_class C;
        C c;
        int i = 0;
        
        utt_assert.is_false(convertor(c, i), CODE_POSITION());
        utt_assert.is_false(convertor(i, c), CODE_POSITION());
        return true;
    }

public:
    bool run() override
    {
        return str_int_case() &&
               str_bool_case() &&
               int_uint32_case() &&
               int_constructable_class_case() &&
               int_assignable_class_case() &&
               no_int_class_case();
    }

    DEFINE_CASE(convertor_test);
};
REGISTER_CASE(convertor_test);

