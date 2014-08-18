
#pragma once
#include "../app_info/assert.hpp"
#include <functional>
#include <utility>
#include "queue_runner.hpp"
#include "../template/singleton.hpp"

const static class queue_runner_once_t
{
private:
    template <template <typename T> class Qless>
    class once_queuer : public queue_runner_t<Qless>::queuer
    {
    private:
        const std::function<void(void)> f;
    public:
        once_queuer(std::function<void(void)>&& f) :
            f(std::move(f)) { }

        bool operator()() override
        {
            f();
            return false;
        }
    };

    queue_runner_once_t() { }

public:
    template <template <typename T> class Qless>
    bool operator()(queue_runner_t<Qless>& q,
                    std::function<void(void)>&& f) const
    {
        return assert(q.push(new once_queuer<Qless>(std::move(f))));
    }

    CONST_SINGLETON(queue_runner_once_t);
}& queue_runner_once = queue_runner_once_t::instance();

