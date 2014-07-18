
#include <atomic>
#include "../app_info/assert.hpp"
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

        atomic<int> v;
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
        node* next;
        T v;
        value_status vs;

        node(T&& v) :
            next(nullptr),
            v(std::move(v)),
            vs() { }

        node(const T& v) :
            next(nullptr),
            v(v),
            vs() { }
    };

    node f;
    node* e;

    void wait_mark_writting()
    {
        // TODO: slimqless, wait_until
        wait_when([this]()
                  {
                      assert(e != nullptr);
                  });
    }

    void wait_mark_written()
    {
    }

    void push(node&& n)
    {
    }

public:
    slimqless()
    {
        e = new node();
        f.next = e;
    }

    void push(const T& v)
    {
    }

    void push(T&& v)
    {
    }

    template <typename... Args>
    void emplace_push(Args&&... args)
    {
    }

    bool pop(T& v)
    {
    }
};

