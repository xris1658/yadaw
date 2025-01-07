#include "audio/engine/AudioEngineWorkerThreadPool.hpp"

#include <chrono>
#include <cstdio>
#include <memory>
#include <numeric>
#include <thread>
#include <vector>

int main()
{
    YADAW::Audio::Engine::AudioEngineWorkerThreadPool pool(
        std::make_unique<YADAW::Audio::Engine::ProcessSequenceWithPrev>()
    );
    std::vector<std::uint16_t> affinities(8, 0);
    std::iota(affinities.begin(), affinities.end(), 0);
    pool.setAffinities(affinities);
    pool.start();
    std::atomic_flag stop;
    std::thread audioCallbackThread(
        [&stop, &pool]()
        {
            while(!stop.test(std::memory_order_acquire))
            {
                using std::literals::chrono_literals::operator ""ms;
                auto next = std::chrono::steady_clock::now() + 10ms;
                pool.mainFunc();
                std::this_thread::sleep_until(next);
            }
        }
    );
    std::getchar();
    pool.updateProcessSequence(
        std::make_unique<YADAW::Audio::Engine::ProcessSequenceWithPrev>()
    );
    std::getchar();
    stop.test_and_set(std::memory_order_release);
    pool.stop();
    audioCallbackThread.join();
}