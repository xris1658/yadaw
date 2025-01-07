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
        workerThreadTaskStatus_.resize(
            affinities_.size() - 1, WorkerThreadTaskStatus::NoWorker
        );
        atomicWorkerThreadTaskStatus_.clear();
        atomicWorkerThreadTaskStatus_.reserve(affinities_.size() - 1);
        FOR_RANGE0(i, affinities_.size() - 1)
        {
            atomicWorkerThreadTaskStatus_.emplace_back(
                workerThreadTaskStatus_[i]
            );
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
            if(thread.joinable())
            {
                thread.join();
            }
        }
        firstCallback_.clear(std::memory_order_release);
        atomicWorkerThreadTaskStatus_.clear();
        workerThreadTaskStatus_.clear();
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
                static_cast<std::uint64_t>(1) << affinities_.back()
            );
        }
    }
    workerThreadDoneCounter_.store(0, std::memory_order_release);
    // FIXME: The audio callback thread waits for worker threads that might be
    //        not awaken. See the following comment.
    //        It might be a good idea to split tasks into smaller ones and do
    //        those in the audio callback thread.
    workerFunc(affinities_.back());
    FOR_RANGE0(i, workerThreads_.size())
    {
        // Since `atomic<T>::wait` calls system APIs that work in kernel mode,
        // worker threads might wake up later than we'd expect. To handle this,
        // the audio callback thread do the tasks that should have been done in
        // worker threads, until the worker threads are ready for doing tasks.
        // Since we use `workerFunc` directly, some unnecessary spin lock loops
        if(auto& status = atomicWorkerThreadTaskStatus_[i];
            status.load(std::memory_order_acquire)
            != WorkerThreadTaskStatus::ProcessedByWorkerThread)
        {
            status.store(
                WorkerThreadTaskStatus::ProcessedByAudioCallbackThread,
                std::memory_order_release
            );
            workerFunc(i);
            status.store(
                WorkerThreadTaskStatus::NoWorker,
                std::memory_order_release
            );
            workerThreadDoneCounter_.fetch_add(1, std::memory_order_release);
        }
        else
        {
            while(workerThreadDoneCounter_.load(std::memory_order_acquire) == i)
            {
                YADAW::Native::inSpinLockLoop();
            }
        }
    }
}

void AudioEngineWorkerThreadPool::updateProcessSequence(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev)
{
    processSequenceWithPrev_.update(std::move(processSequenceWithPrev));
    // TODO
}

void AudioEngineWorkerThreadPool::workerThreadFunc(
    std::uint32_t processorIndex, std::uint32_t workloadIndex)
{
    YADAW::Native::setThreadAffinity(
        YADAW::Native::getCurrentThreadHandle(),
        static_cast<std::uint64_t>(1) << processorIndex
    );
    running_.wait(false, std::memory_order_acquire);
    // See comments in `AudioEngineWorkerThreadPool::mainFunc`
    auto& status = atomicWorkerThreadTaskStatus_[workloadIndex];
    while(status.load(std::memory_order_acquire)
        == WorkerThreadTaskStatus::ProcessedByAudioCallbackThread)
    {
        YADAW::Native::inSpinLockLoop();
    }
    status.store(WorkerThreadTaskStatus::ProcessedByWorkerThread,
        std::memory_order_release
    );
    while(running_.test(std::memory_order_acquire))
    {
        while(workerThreadDoneCounter_.load(std::memory_order_acquire) != 0)
        {
            YADAW::Native::inSpinLockLoop();
        }
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
        auto& [processPairs, prev] = processSeq[row][col];
        for(const auto& [prevRow, prevCol]: prev)
        {
            while(workload->atomicCompletionMarks[prevRow][prevCol].load(
                std::memory_order_acquire) == false
            )
            {
                YADAW::Native::inSpinLockLoop();
            }
        }
        for(auto& [process, buffer]: processPairs)
        {
            process.process(buffer.audioProcessData());
        }
        workload->atomicCompletionMarks[row][col].store(true, std::memory_order_release);
    }
}
}
