#include "native/Thread.hpp"

#include <cmath>
#include <thread>

int main()
{
    std::atomic_flag run;
    run.test_and_set();
    std::thread thread([&run]()
    {
        auto i = 0.0;
        while(run.test(std::memory_order_acquire))
        {
            auto sine = std::sin(i);
            // std::printf("std::sin(%lf) = %lf\n", i, sine);
            i+= 0.125;
        }
    });
    if(YADAW::Native::setThreadAffinity(thread.native_handle(), 1 << 4))
    {
        std::getchar();
    }
    run.clear(std::memory_order_release);
    thread.join();
}