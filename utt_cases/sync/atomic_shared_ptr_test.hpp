
#pragma once
#include "../../app_info/assert.hpp"
#include "../../sync/atomic_shared_ptr.hpp"
#include "../../utt/icase.hpp"
#include "../../utt/case_wrapper/repeat_case_wrapper.hpp"
#include "../../utt/case_wrapper/multithreading_case_wrapper.hpp"
#include "../../utils/cd_object.hpp"
#include <atomic>
#include <stdint.h>
#include "../../utils/random.hpp"
#include "../../app_info/trace.hpp"
#include "../../envs/os.hpp"

class atomic_shared_ptr_case : public icase
{
private:
    class C
    {
    private:
        std::atomic<uint32_t> a;
    public:
        C() : a(0) { }

        uint32_t increment()
        {
            return a.fetch_add(1, std::memory_order_consume);
        }

        uint32_t operator*() const
        {
            return a.load();
        }

        void reset()
        {
            a = 0;
        }
    };

    atomic_shared_ptr<cd_object<C>> root;
    std::atomic<uint32_t> times;

public:
    atomic_shared_ptr_case() : times(0) { }

    bool prepare() override
    {
        compiler_barrier();
        times = 0;
        root.reset();
        cd_object<C>::reset();
        root.reset(new cd_object<C>());
        return icase::prepare();
    }

    bool execute() override
    {
        atomic_shared_ptr<cd_object<C>>* t = nullptr;
        times.fetch_add(1, std::memory_order_consume);
        if(rnd_bool())
        {
            // operator= (const atomic_shared_ptr<cd_object<C>>&)
            t = new atomic_shared_ptr<cd_object<C>>();
            t->operator=(root);
        }
        else if(rnd_bool())
        {
            // ctor(const atomic_shared_ptr<cd_object<C>>&)
            t = new atomic_shared_ptr<cd_object<C>>(root);
        }
        else if(rnd_bool())
        {
            // operator= (const std::shared_ptr<cd_object<C>>&)
            t = new atomic_shared_ptr<cd_object<C>>();
            t->operator=(std::shared_ptr<cd_object<C>>(root));
        }
        else
        {
            t = new atomic_shared_ptr<cd_object<C>>(std::shared_ptr<cd_object<C>>(root));
        }
        assert(t != nullptr);
        utt_assert.more(t->use_count(), 1);
        utt_assert.is_false(t->unique());
        t->get()->ref().increment();
        delete t;
        return true;
    }

    bool cleanup() override
    {
        utt_assert.equal(times.load(),
                         *(root->ref()),
                         ", ",
                         times.load(),
                         ", ",
                         *(root->ref()),
                         CODE_POSITION());
        utt_assert.equal(cd_object<C>::create_times(),
                         1,
                         ", ",
                         cd_object<C>::create_times(),
                         CODE_POSITION());
        utt_assert.equal(cd_object<C>::destruct_times(),
                         0,
                         ", ",
                         cd_object<C>::destruct_times(),
                         CODE_POSITION());
        utt_assert.equal(cd_object<C>::instance_count(),
                         1,
                         ", ",
                         cd_object<C>::instance_count(),
                         CODE_POSITION());
        utt_assert.is_true(root.unique());
        utt_assert.equal(root.use_count(), 1);
        return icase::cleanup();
    }

    DEFINE_CASE(atomic_shared_ptr_case);
};

class atomic_shared_ptr_test : public
        rinne_case_wrapper<
            multithreading_case_wrapper<
                repeat_case_wrapper<atomic_shared_ptr_case,
                                    1024>,
                8>,
            1024>
{
    DEFINE_CASE(atomic_shared_ptr_test);
};
REGISTER_CASE(atomic_shared_ptr_test);

