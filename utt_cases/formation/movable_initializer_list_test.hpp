
#pragma once
#include <stdlib.h>
#include "../../formation/movable_initializer_list.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/utt_assert.hpp"
#include "../../utt/fake/construct_counter.hpp"
#include "../../app_info/assert.hpp"
#include "../../app_info/trace.hpp"

class movable_initializer_list_test : public icase
{
private:
    class diff_type { };

public:
    bool run() override
    {
        typedef construct_counter<diff_type> cc;
        cc::reset();
        cc* f = new cc();
        cc* s = new cc();
        cc* t = new cc();
        assert(f->index == 0);
        assert(s->index == 1);
        assert(t->index == 2);
        std::initializer_list<cc*> il({ f, s, t} );
        f = nullptr;
        s = nullptr;
        t = nullptr;
        {
            size_t c = 0;
            for(auto it = il.begin(); it != il.end(); it++)
            {
                assert((*it) != nullptr);
                assert((*it)->index == c);
                c++;
            }
        }

        std::movable_initializer_list<cc> mil(il);
        utt_assert.equal(mil.size(), 3, CODE_POSITION());
        {
            size_t c = 0;
            for(auto it = mil.begin(); it != mil.end(); it++)
            {
                utt_assert.equal((*it).index, c + 6, CODE_POSITION());
                c++;
            }
        }
        utt_assert.equal(cc::default_constructed(), 3, CODE_POSITION());
        utt_assert.equal(cc::move_constructed(), 6, CODE_POSITION());
        utt_assert.equal(cc::destructed(), 6, CODE_POSITION());
        utt_assert.equal(cc::instance_count(), 3, CODE_POSITION());
        return true;
    }

    DEFINE_CASE(movable_initializer_list_test);
};

REGISTER_CASE(movable_initializer_list_test);

