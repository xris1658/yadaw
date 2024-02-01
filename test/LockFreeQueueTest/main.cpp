#include "util/FixedSizeLockFreeQueue.hpp"
#include "util/LockFreeQueue.hpp"

#include <cassert>
#include <cstdio>
#include <thread>

constexpr auto count = 0x7fff;

void lockFreeQueueTest()
{
    YADAW::Util::LockFreeQueue<int> queue(40);
    std::thread producer([&queue]()
    {
        for(int i = 0; i < count; ++i)
        {
            // std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
            while(!queue.emplace(i)) {}
        }
    });
    std::thread consumer([&queue]()
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int value = -1;
        for(int i = 0; i < count; ++i)
        {
            // std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
            while(!queue.popTo(value)) {}
            assert(value == i);
            std::printf("%d, ", value);
        }
    });
    producer.join();
    consumer.join();
}

void fixedSizeLockFreeQueueTest()
{
    YADAW::Util::FixedSizeLockFreeQueue<int, 40> queue;
    std::thread producer([&queue]()
    {
        for(int i = 0; i < count; ++i)
        {
            // std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
            while(!queue.emplace(i)) {}
        }
    });
    std::thread consumer([&queue]()
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int value = -1;
        for(int i = 0; i < count; ++i)
        {
            // std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 20));
            while(!queue.popTo(value)) {}
            assert(value == i);
            std::printf("%d, ", value);
        }
    });
    producer.join();
    consumer.join();
}

int main()
{
    lockFreeQueueTest();
    fixedSizeLockFreeQueueTest();
}