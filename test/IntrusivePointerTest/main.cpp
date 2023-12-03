#include "util/IntegerRange.hpp"
#include "util/IntrusivePointer.hpp"

#include <cassert>
#include <new> // std::nothrow
#include <thread>
#include <vector>

class IntrusiveInt: public YADAW::Util::IntrusiveRefCounter
{
private:
    IntrusiveInt(int value): value_(value) {}
public:
    static YADAW::Util::IntrusivePointer<IntrusiveInt> create(int value)
    {
        return YADAW::Util::IntrusivePointer<IntrusiveInt>(new(std::nothrow) IntrusiveInt(value));
    }
    operator int&()
    {
        return value_;
    }
    YADAW::Util::IntrusivePointer<IntrusiveInt> intrusiveFromThis()
    {
        addRef();
        return YADAW::Util::IntrusivePointer<IntrusiveInt>(this);
    }
private:
    int value_ = 0;
};

int main()
{
    auto intrusiveInt = IntrusiveInt::create(42);
    auto intrusiveInt2 = intrusiveInt;
    {
        auto& value = static_cast<int&>(*intrusiveInt2);
        value = 52;
        assert(*intrusiveInt == 52);
    }
    auto intrusiveInt3 = intrusiveInt->intrusiveFromThis();
    {
        auto& value = static_cast<int&>(*intrusiveInt3);
        value = 42;
        assert(*intrusiveInt2 == 42);
        assert(*intrusiveInt == 42);
    }
    intrusiveInt3.reset();
    intrusiveInt2.reset();
    auto threadCount = std::thread::hardware_concurrency();
    std::vector<std::pair<std::thread, YADAW::Util::IntrusivePointer<IntrusiveInt>>> vec;
    vec.reserve(threadCount);
    bool del = true;
    FOR_RANGE0(i, threadCount)
    {
        vec.emplace_back(
            std::make_pair(
                std::thread(),
                intrusiveInt
            )
        );
        vec[i].first = std::thread(
            [&del, &intrusivePtr = vec[i].second]()
            {
                while(del) {}
                intrusivePtr.reset();
            }
        );
    }
    intrusiveInt.reset();
    del = false;
    for(auto& [thread, intrusivePtr]: vec)
    {
        thread.join();
    }
    return 0;
}