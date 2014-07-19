
#pragma once
#include <utility>
#include <atomic>
#include "../../app_info/assert.hpp"
#include "../../sync/spin_wait.hpp"

template <typename T>
class slimqless
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
                   assert(exp == nv);
        }

        void mark_value_written()
        {
            assert(v == bw);
            v = aw;
        }

        bool not_no_value() const
        {
            return v != nv;
        }

        bool value_written() const
        {
            assert(not_no_value());
            return v == aw;
        }
    };

    struct node
    {
    public:
        std::atomic<node*> next;
        T v;
        value_status vs;

        node() :
            next(nullptr),
            v(),
            vs() { }
    };

    std::atomic<node*> f;
    std::atomic<node*> e;

    inline void wait_mark_writting()
    {
        std::this_thread::busy_wait_until(
                [this]() -> bool
                {
                    assert(e.load() != nullptr);
                    return e.load()->vs.mark_value_writting();
                });
    }

    inline static void wait_mark_written(const node* n)
    {
        assert(n != nullptr);
        std::this_thread::busy_wait_until(
                [&]() -> bool
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
        n->next = ne;
        e = ne;
        new (&(n->v)) T(std::forward<Args>(args)...);
        n->vs.mark_value_written();
    }

public:
    slimqless()
    {
        f = new node();
        e = new node();
        f.load()->next.store(e);
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

    bool pop(T& v)
    {
        node* nf = f.load()->next;
        while(1)
        {
            assert(nf != nullptr);
            if(nf == e) return false;
            else
            {
                node* t = nf;
                if(f.load()->next.compare_exchange_weak(t, nf->next) &&
                   assert(t == nf))
                {
                    assert(nf->vs.not_no_value());
                    wait_mark_written(nf);
                    new (&v) T(std::move(nf->v));
                    return true;
                }
                else nf = f.load()->next;
            }
        }
        return assert(false);
    }
};

