
#pragma once
#include <utility>
#include <atomic>
#include <memory>
#include "../../app_info/assert.hpp"
#include "../../app_info/trace.hpp"
#include "../../sync/spin_wait.hpp"
#include "../../sync/shared_atomic.hpp"

template <typename T>
class slimqless final
{
private:
    struct value_status
    {
    private:
        const static int nv = 0;
        const static int bw = 1;
        const static int aw = 2;

        std::atomic<int> v;
    public:
        value_status() : v(nv) { }

        bool mark_value_writting()
        {
            int exp = nv;
            return v.compare_exchange_weak(exp, bw) &&
                   assert(exp == nv, CODE_POSITION());
        }

        void mark_value_written()
        {
            assert(v == bw, CODE_POSITION(), ", ", v);
            v = aw;
        }

        bool not_no_value() const
        {
            return v != nv;
        }

        bool value_written() const
        {
            assert(not_no_value(), CODE_POSITION(), ", ", v);
            return v == aw;
        }
    };

    struct node
    {
    public:
        shared_atomic_ptr<node> next;
        T v;
        value_status vs;
    };

    node f;
    std::atomic<node*> e;

    inline void wait_mark_writting()
    {
        std::this_thread::strict_wait_until(
                [this]()
                {
                    assert(e.load() != nullptr, CODE_POSITION());
                    return e.load()->vs.mark_value_writting();
                });
    }

    inline static void wait_mark_written(const node* n)
    {
        assert(n != nullptr, CODE_POSITION());
        std::this_thread::strict_wait_until(
                [&]()
                {
                    return n->vs.value_written();
                });
    }

    template <typename... Args>
    void insert(Args&&... args)
    {
        node* ne = new node();
        wait_mark_writting();
        node* n = e;
        n->next.set(ne);
        e = ne;
        new (&(n->v)) T(std::forward<Args>(args)...);
        n->vs.mark_value_written();
    }

public:
    slimqless()
    {
        e = new node();
        f.next.set(e);
    }

    void push(const T& v)
    {
        insert(v);
    }

    void push(T&& v)
    {
        insert(std::forward<T>(v));
    }

    template <typename... Args>
    void emplace_push(Args&&... args)
    {
        insert(std::forward<Args>(args)...);
    }

    bool empty() const
    {
        return f.next.load() == e;
    }

    bool pop(T& v)
    {
        shared_atomic_ptr<node> nf = f.next;
        bool failed = false;
        std::this_thread::strict_wait_when(
                [&]()
                {
                     assert(nf.load() != nullptr, CODE_POSITION());
                     if(nf.load() == e)
                     {
                         failed = true;
                         return false;
                     }
                     else
                     {
                         assert(nf.load()->next.load() != nullptr,
                                CODE_POSITION(),
                                ", ",
                                nf.load(),
                                ", ",
                                e.load(),
                                ", ",
                                nf.load()->next.load());
                         node* t = nf.load();
                         if(f.next.compare_exchange_weak(t, nf.load()->next.load()) &&
                            assert(t == nf.load(), CODE_POSITION(), ", ", t, ", ", nf.load())) return false;
                         else
                         {
                             nf = f.next;
                             return true;
                         }
                     }
                });
        if(failed) return false;
        else
        {
            wait_mark_written(nf.load());
            new (&v) T(std::move(nf.load()->v));
            return true;
        }
    }

    void clear()
    {
        T v;
        while(pop(v));
    }

    ~slimqless()
    {
        clear();
    }
};

