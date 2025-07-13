#include "AudioEngineWorkerThreadPool.hpp"

#include "native/AudioEngineWorkerThreadScheduler.hpp"
#include "native/CPU.hpp"
#include "native/Thread.hpp"

template<typename T>
T* getType(std::vector<YADAW::Util::AlignedStorage<T>>& vec, typename std::vector<YADAW::Util::AlignedStorage<T>>::size_type index)
{
    return YADAW::Util::AlignHelper<T>::fromAligned(vec.data() + index);
}

namespace YADAW::Audio::Engine
{
void blankSetAudioThreadIdCallback(const AudioEngineWorkerThreadPool&, std::thread::id)
{}

void blankUnsetAudioThreadIdCallback(const AudioEngineWorkerThreadPool&, std::thread::id)
{}

AudioEngineWorkerThreadPool::Workload::Workload(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev,
    std::uint32_t threadCount
):
    processSequenceWithPrev(std::move(processSequenceWithPrev)),
    audioThreadWorkload(YADAW::Audio::Engine::createWorkload(*(this->processSequenceWithPrev), threadCount))
{
    completionMarksStorage.resize(this->processSequenceWithPrev->size());
    FOR_RANGE0(i, completionMarksStorage.size())
    {
        completionMarksStorage[i].resize((*(this->processSequenceWithPrev))[i].size());
        for(auto& storage: completionMarksStorage[i])
        {
            auto* pDone = YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(&storage);
            new(pDone) std::atomic_flag();
        }
    }
}

AudioEngineWorkerThreadPool::Workload::~Workload()
{}

AudioEngineWorkerThreadPool::AudioEngineWorkerThreadPool(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev
):
    workload_(std::make_unique<Workload>(std::move(processSequenceWithPrev), 1)),
    setAudioThreadIdCallback_(&blankSetAudioThreadIdCallback),
    unsetAudioThreadIdCallback_(&blankUnsetAudioThreadIdCallback)
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
        workload_.updateAndGetOld(
            std::make_unique<Workload>(
                std::move(workload_.get()->processSequenceWithPrev),
                affinities_.size()
            ),
            false
        );
        if constexpr(!std::is_trivially_destructible_v<std::atomic_flag>)
        {
            for(auto& storage: workerThreadDoneStorage_)
            {
                using std::atomic_flag;
                // is std::atomic_flag trivially destructible?
                YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(&storage)->~atomic_flag();
            }
        }
        workerThreadDoneStorage_.resize(affinities.size() - 1);
        for(auto& storage: workerThreadDoneStorage_)
        {
            auto* pDone = YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(&storage);
            new(pDone) std::atomic_flag();
        }
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
        firstCallback_.test_and_set(std::memory_order_release);
        FOR_RANGE0(i, affinities_.size() - 1)
        {
            workerThreads_.emplace_back(
                [this, i]()
                {
                    workerThreadFunc(affinities_[i], i);
                }
            );
        }
        for(auto& thread: workerThreads_)
        {
            setAudioThreadIdCallback_(*this, thread.get_id());
        }
    }
    return true;
}

void AudioEngineWorkerThreadPool::stop()
{
    if(running())
    {
        running_.clear(std::memory_order_release);
        for(auto& storage: workerThreadDoneStorage_)
        {
            auto& done = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(&storage);
            done.clear(std::memory_order_release);
            done.notify_one();
        }
        for(auto& thread: workerThreads_)
        {
            if(thread.joinable())
            {
                auto id = thread.get_id();
                thread.join();
                unsetAudioThreadIdCallback_(*this, id);
            }
        }
        unsetAudioThreadIdCallback_(*this, mainCallbackThreadId_);
        mainCallbackThreadId_ = {};
    }
    mainAffinityIsSet_ = false;
    firstCallback_.test_and_set(std::memory_order_release);
}

void AudioEngineWorkerThreadPool::mainFunc()
{
    if(firstCallback_.test(std::memory_order_acquire))
    {
        mainCallbackThreadId_ = std::this_thread::get_id();
        setAudioThreadIdCallback_(*this, mainCallbackThreadId_);
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
        firstCallback_.clear(std::memory_order_release);
    }
    if(workload_.swapIfNeeded())
    {
        std::fprintf(stderr, "{");
    }
    auto& completionMarks = workload_.get()->completionMarksStorage;
    for(auto& row: completionMarks)
    {
        for(auto& cell: row)
        {
            auto& completionMark = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(&cell);
            completionMark.clear(std::memory_order_relaxed);
        }
    }
    auto& workload = workload_.get()->audioThreadWorkload;
    FOR_RANGE(i, 0, workload.size() - 1)
    {
        if(!workload[i].empty())
        {
            auto& done = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(workerThreadDoneStorage_.data() + i);
            done.clear(std::memory_order_release);
            done.notify_one();
        }
    }
    workerFunc(affinities_.size() - 1);
    FOR_RANGE(i, 0, workload.size() - 1)
    {
        if(!workload[i].empty())
        {
            auto& done = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(workerThreadDoneStorage_.data() + i);
            done.wait(false, std::memory_order_acquire);
        }
    }
}

void AudioEngineWorkerThreadPool::updateProcessSequence(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev)
{
    workload_.updateAndGetOld(
        std::make_unique<Workload>(
            std::move(processSequenceWithPrev),
            affinities_.size()
        ),
        running()
    );
    std::fprintf(stderr, "}\n");
}

void AudioEngineWorkerThreadPool::setManageAudioThreadIdCallback(
    std::function<SetAudioThreadIdCallback>&& setAudioThreadIdCallback,
    std::function<UnsetAudioThreadIdCallback>&& unsetAudioThreadIdCallback)
{
    setAudioThreadIdCallback_ = std::move(setAudioThreadIdCallback);
    unsetAudioThreadIdCallback_ = std::move(unsetAudioThreadIdCallback);
}

void AudioEngineWorkerThreadPool::resetManageAudioThreadIdCallback()
{
    setAudioThreadIdCallback_ = &blankSetAudioThreadIdCallback;
    unsetAudioThreadIdCallback_ = &blankUnsetAudioThreadIdCallback;
}

void AudioEngineWorkerThreadPool::workerThreadFunc(
    std::uint32_t processorIndex, std::uint32_t workloadIndex)
{
    ThreadScheduler threadScheduler(*this, InitArgs()/*TODO*/);
    YADAW::Native::setThreadAffinity(
        YADAW::Native::getCurrentThreadHandle(),
        static_cast<std::uint64_t>(1) << processorIndex
    );
    running_.wait(false, std::memory_order_acquire);
    do
    {
        workerFunc(workloadIndex);
        auto& done = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(workerThreadDoneStorage_.data() + workloadIndex);
        done.test_and_set(std::memory_order_release);
        done.notify_one();
        done.wait(true, std::memory_order_acquire);
    }
    while(running_.test(std::memory_order_acquire));
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
            auto& done = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(workload->completionMarksStorage[prevRow].data() + prevCol);
            while(done.test(std::memory_order_acquire) == false)
            {
                YADAW::Native::inSpinLockLoop();
            }
        }
        for(auto& [process, buffer]: processPairs)
        {
            process.process(buffer.audioProcessData());
        }
        auto& done = *YADAW::Util::AlignHelper<std::atomic_flag>::fromAligned(workload->completionMarksStorage[row].data() + col);
        done.test_and_set(std::memory_order_release);
    }
}
}
