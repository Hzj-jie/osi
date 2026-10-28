
#pragma once
#include "../../utt/utt_assert.hpp"
#include "../../utt/icase.hpp"
#include "../../sync/reset_event.hpp"
#include "../../sync/yield.hpp"
#include <atomic>
#include <stdint.h>
#include <vector>
#include <thread>

class reset_event_test : public icase
{
private:
    const static int waiters_thread_count = 2;

    template <typename T>
    static void run_waiter(const std::reference_wrapper<T>& re,
                           const std::reference_wrapper<std::atomic<int64_t>>& v,
                           const std::reference_wrapper<std::atomic<bool>>& r,
                           const std::reference_wrapper<std::atomic<uint32_t>>& finished)
    {
        while(r.get().load(std::memory_order_acquire))
        {
            re.get().wait();
            v.get().fetch_add(1, std::memory_order_acq_rel);
        }
        finished.get().fetch_add(1, std::memory_order_acq_rel);
    }

    template <typename T>
    static std::vector<std::thread> start_waiters(T& re,
                                                  std::atomic<int64_t>& v,
                                                  std::atomic<bool>& r,
                                                  std::atomic<uint32_t>& finished)
    {
        using namespace std;
        vector<thread> vs;
        for(int i = 0; i < waiters_thread_count; i++)
            vs.push_back(thread(run_waiter<T>, ref(re), ref(v), ref(r), ref(finished)));
        return vs;
    }

    template <typename T>
    static void wait_for_waiters(T&& re,
                                 std::vector<std::thread>& v,
                                 std::atomic<bool>& r,
                                 std::atomic<uint32_t>& finished)
    {
        r.store(false, std::memory_order_release);
        while(finished.load(std::memory_order_acquire) < v.size())
        {
            re.set();
            std::this_thread::interval();
        }
        for(size_t i = 0; i < v.size(); i++) v[i].join();
        v.clear();
    }

    static void pending()
    {
        for(int i = 0; i < 2; i++) std::this_thread::interval();
    }

    bool auto_reset_event_test()
    {
        using namespace std;
        auto_reset_event are;
        utt_assert.is_true(are.reset());
        atomic<int64_t> v(0);
        atomic<bool> running(true);
        atomic<uint32_t> finished(0);
        vector<thread>&& vs = start_waiters(are, v, running, finished);
        for(int i = 0; i < 1000; i++)
        {
            utt_assert.equal(v.load(memory_order_acquire), i);
            utt_assert.is_true(are.set());
            while(v.load(memory_order_acquire) < i + 1) std::this_thread::yield();
            for(int j = 0; j < 2; j++)
            {
                std::this_thread::yield();
                utt_assert.is_false(are);
                utt_assert.equal(v.load(memory_order_acquire), i + 1);
            }
        }
        wait_for_waiters(are, vs, running, finished);
        return true;
    }

    bool manual_reset_event_test()
    {
        using namespace std;
        manual_reset_event mre;
        utt_assert.is_true(mre.reset());
        atomic<int64_t> v(0);
        atomic<bool> running(true);
        atomic<uint32_t> finished(0);
        vector<thread>&& vs = start_waiters(mre, v, running, finished);
        for(int i = 0; i < 200; i++)
        {
            int64_t c = v.load(memory_order_acquire);
            utt_assert.is_true(mre.set());
            for(int j = 0; j < 2; j++)
            {
                c = v.load(memory_order_acquire);
                while(v.load(memory_order_acquire) <= c) std::this_thread::yield();
                utt_assert.is_true(mre);
                utt_assert.more(v.load(memory_order_acquire), c);
            }
            utt_assert.is_true(mre.reset());
            pending();
            c = v.load(memory_order_acquire);
            for(int j = 0; j < 2; j++)
            {
                pending();
                utt_assert.is_false(mre);
                utt_assert.equal(v.load(memory_order_acquire), c);
            }
        }
        wait_for_waiters(mre, vs, running, finished);
        return true;
    }

public:
    uint32_t preserved_processor_count() const override
    {
        return icase::preserved_processor_count() + waiters_thread_count;
    }

    bool run() override
    {
        return auto_reset_event_test() &&
               manual_reset_event_test();
    }

    DEFINE_CASE(reset_event_test);
};

REGISTER_CASE(reset_event_test);

