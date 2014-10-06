
#pragma once
#include "../threadpool/threadpool.hpp"
#include "../threadpool/queue_runner.hpp"
#include <mutex>
#include <string>
#include <stdint.h>
#include <functional>
#include <vector>
#include "../app_info/trace.hpp"
#include "../app_info/assert.hpp"
#include "../formation/ternary.hpp"
#include "../delegates/event.hpp"
#include "../threadpool/stopwatch.hpp"
#include <utility>
#include "../utils/call_stack.hpp"
#include <memory>
#include <mutex>
#include "../formation/movable_initializer_list.hpp"

class event_comb
{
public:
    typedef std::shared_ptr<event_comb> event_comb_type;
    const code_position ctor_position;

private:
    const static int end_step = INT_MAX;
    const static int not_started_step = -1;
    const static int first_step = 0;
    std::vector<std::function<bool(void)>> ds;
    ternary _end_result;
    stopwatch_event timeout_event;
    event_comb_type cb;
    int step;
    uint32_t pends;
    int64_t _begin_ms;
    int64_t _end_ms;
    std::recursive_mutex _l;

protected:
    event<> suspending;

private:
    event_comb(const std::movable_initializer_list<std::function<bool(void)>>& d) :
        ctor_position(CODE_POSITION())
    {
        for(auto it = d.begin(); it != d.end(); it++)
            ds.push_back(std::move(*it));
    }

    void assert_in_lock()
    {
        // TODO: detect whether the lock is held in current thread
    }

    void clear_pends()
    {
        assert_in_lock();
        pends = 0;
    }

    void begin_ms(int64_t v)
    {
        assert(_begin_ms == 0 || v == 0);
        _begin_ms = v;
    }

    void end_ms(int64_t v)
    {
        assert(_end_ms == 0 || v == 0);
        _end_ms = v;
    }

    void _do()
    {
    }

public:
    template <typename... Args>
    static event_comb_type create(Args&&... args)
    {
        return event_comb_type(new event_comb(std::forward<Args>(args)...));
    }

    static event_comb& current()
    {
        return (*call_stack<event_comb_type>().current());
    }

    const ternary& end_result_raw() const
    {
        return _end_result;
    }

    bool end_result() const
    {
        assert(end_result_raw().notunknown());
        return end_result_raw().true_();
    }

    bool pending() const
    {
        return pends > 0;
    }

    bool not_pending() const
    {
        return pends == 0;
    }

    int64_t begin_ms() const
    {
        return _begin_ms;
    }

    int64_t end_ms() const
    {
        return _end_ms;
    }

protected:
    static void current(event_comb* x)
    {
        call_stack<event_comb*>().push(x);
    }

    const std::string& callstack() const
    {
        return ctor_position.str();
    }

    void inc_pends()
    {
        assert_in_lock();
        pends++;
    }

    void dec_pends()
    {
        assert_in_lock();
        assert(pends > 0);
        pends--;
    }

    void mark_as_failed()
    {
        _end_result = false;
    }
};

