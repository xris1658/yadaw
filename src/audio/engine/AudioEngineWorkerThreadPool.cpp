#include "AudioEngineWorkerThreadPool.hpp"

#include "native/CPU.hpp"
#include "native/Thread.hpp"

namespace YADAW::Audio::Engine
{
AudioEngineWorkerThreadPool::Workload::Workload(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev
):
    processSequenceWithPrev(std::move(processSequenceWithPrev))
{
    completionMarks.resize(this->processSequenceWithPrev->size());
    atomicCompletionMarks.resize(this->processSequenceWithPrev->size());
    FOR_RANGE0(i, completionMarks.size())
    {
        completionMarks[i].resize((*(this->processSequenceWithPrev))[i].size());
        atomicCompletionMarks[i].reserve(completionMarks[i].size());
        FOR_RANGE0(j, completionMarks[i].size())
        {
            atomicCompletionMarks[i].emplace_back(completionMarks[i][j]);
        }
    }
}

AudioEngineWorkerThreadPool::Workload::~Workload()
{}

AudioEngineWorkerThreadPool::AudioEngineWorkerThreadPool(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev
):
    processSequenceWithPrev_(nullptr),
    workload_(std::make_unique<Workload>(std::move(processSequenceWithPrev))),
    updateCounter_(0)
{}

AudioEngineWorkerThreadPool::~AudioEngineWorkerThreadPool()
{
    stop();
}

bool AudioEngineWorkerThreadPool::running() const
{
    return running_.test(std::memory_order_acquire);
}

bool AudioEngineWorkerThreadPool::setAffinities(
    const std::vector<std::uint16_t>& affinities)
{
    if((!running()) && (!affinities.empty()))
    {
        affinities_ = affinities;
        workerThreads_.clear();
        workerThreads_.reserve(affinities.size() - 1);
        return true;
    }
    return false;
}

std::uint32_t AudioEngineWorkerThreadPool::getMainAffinity() const
{
    auto ret = affinities_.empty()? 0: affinities_.front();
    mainAffinityIsSet_ = true;
    return ret;
}

bool AudioEngineWorkerThreadPool::start()
{
    if(affinities_.empty())
    {
        return false;
    }
    if(!running())
    {
        FOR_RANGE0(i, affinities_.size() - 1)
        {
            workerThreads_.emplace_back(
                [this, i]()
                {
                    workerThreadFunc(affinities_[i], i);
                }
            );
        }
    }
    return true;
}

void AudioEngineWorkerThreadPool::stop()
{
    if(running())
    {
        running_.clear(std::memory_order_release);
        for(auto& thread: workerThreads_)
        {
            thread.join();
        }
        firstCallback_.clear(std::memory_order_release);
    }
    mainAffinityIsSet_ = false;
    firstCallback_.test_and_set(std::memory_order_release);
}

void AudioEngineWorkerThreadPool::mainFunc()
{
    if(!firstCallback_.test(std::memory_order_acquire))
    {
        // This function is called during the first audio callback. Wake up all
        // worker threads.
        running_.test_and_set(std::memory_order_release);
        running_.notify_all();
        if(!mainAffinityIsSet_)
        {
            YADAW::Native::setThreadAffinity(
                YADAW::Native::getCurrentThreadHandle(),
                static_cast<std::uint64_t>(1) << affinities_[0]
            );
        }
    }
    workerThreadDoneCounter_.store(0, std::memory_order_release);
    workerFunc(affinities_.back());
    FOR_RANGE0(i, workerThreads_.size())
    {
        while(workerThreadDoneCounter_.load(std::memory_order_acquire) == i)
        {
            YADAW::Native::inSpinLockLoop();
        }
    }
}

void AudioEngineWorkerThreadPool::updateProcessSequence(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev)
{
    processSequenceWithPrev_.update(std::move(processSequenceWithPrev));
}

void AudioEngineWorkerThreadPool::workerThreadFunc(
    std::uint32_t processorIndex, std::uint32_t workloadIndex)
{
    YADAW::Native::setThreadAffinity(
        YADAW::Native::getCurrentThreadHandle(),
        static_cast<std::uint64_t>(1) << processorIndex
    );
    running_.wait(false, std::memory_order_acquire);
    while(running_.test(std::memory_order_acquire))
    {
        workerFunc(workloadIndex);
        auto counter = workerThreadDoneCounter_.fetch_add(1, std::memory_order_relaxed) + 1;
        workerThreadDoneCounter_.notify_all();
        FOR_RANGE(i, counter, workerThreads_.size() + 1)
        {
            while(workerThreadDoneCounter_.load(std::memory_order_acquire) == i)
            {
                YADAW::Native::inSpinLockLoop();
            }
        }
    }
}

void AudioEngineWorkerThreadPool::workerFunc(std::uint32_t workloadIndex)
{
    auto& workload = workload_.get();
    auto& tasks = workload->audioThreadWorkload[workloadIndex];
    auto& processSeq = *(workload->processSequenceWithPrev);
    for(const auto& [row, col]: tasks)
    {
        workload->atomicCompletionMarks[row][col].wait(false, std::memory_order_acquire);
        auto& [processPairs, prev] = processSeq[row][col];
        for(auto& [process, buffer]: processPairs)
        {
            process.process(buffer.audioProcessData());
        }
    }
}
}
