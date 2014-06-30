
#include <functional>

class sweeper
{
private:
    const std::function<void()> s;
    const std::function<void()> e;
public:
    sweeper(const std::function<void()>& s, const std::function<void()>& e)
        : s(s), e(e)
    {
        s();
    }

    sweeper(std::function<void()>&& s, std::function<void()>&& e)
        : s(s), e(e)
    {
        s();
    }

    ~sweeper()
    {
        e();
    }
};

