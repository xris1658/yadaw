#include "ButlerThread.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Util
{
ButlerThread::ButlerThread(std::size_t taskCapacity):
    tasks_(taskCapacity),
    thread_([this]() { butlerThreadFunc(); }),
    run_(true)
{}

ButlerThread::~ButlerThread()
{
    {
        std::lock_guard<std::mutex> lg(mutex_);
        run_ = false;
        cv_.notify_one();
    }
    thread_.join();
}

bool ButlerThread::addTask(const std::function<void()>& task)
{
    std::unique_lock<std::mutex> lg(mutex_);
    auto ret = tasks_.push(task);
    if(ret)
    {
        cv_.notify_one();
    }
    return ret;
}

bool ButlerThread::addTask(std::function<void()>&& task)
{
    std::unique_lock<std::mutex> lg(mutex_);
    auto ret = tasks_.push(std::move(task));
    if(ret)
    {
        cv_.notify_one();
    }
    return ret;
}

void ButlerThread::butlerThreadFunc()
{
    while(run_)
    {
        std::size_t size = 0;
        {
            std::unique_lock<std::mutex> lg(mutex_);
            cv_.wait(lg, [this]() { return !tasks_.empty() || (!run_); });
            size = tasks_.size();
        }
        FOR_RANGE0(i, size)
        {
            std::function<void()> function;
            auto popTo = tasks_.popTo(function);
            assert(popTo);
            function();
        }
        std::this_thread::yield();
    }
}
}