#include "AudioEngineWorkerThreadPool.hpp"

#include "native/CPU.hpp"
#include "native/Thread.hpp"

namespace YADAW::Audio::Engine
{
YADAW::Native::CPUTopology& getCPUTopology()
{
    static auto ret = YADAW::Native::getCPUTopology();
    return ret;
}

AudioEngineWorkerThreadPool::Workload::Workload(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev
):
    processSequenceWithPrev(std::move(processSequenceWithPrev))
{
    completionMarks.resize(processSequenceWithPrev->size());
    atomicCompletionMarks.resize(processSequenceWithPrev->size());
    FOR_RANGE0(i, completionMarks.size())
    {
        completionMarks[i].resize(processSequenceWithPrev->operator[](i).size());
        atomicCompletionMarks[i].reserve(completionMarks[i].size());
        FOR_RANGE0(j, completionMarks[i].size())
        {
            atomicCompletionMarks[i].emplace_back(completionMarks[i][j]);
        }
    }
}

AudioEngineWorkerThreadPool::Workload::~Workload()
{
}

AudioEngineWorkerThreadPool::AudioEngineWorkerThreadPool(
    std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev
):
    processSequenceWithPrev_(nullptr),
    workload_(std::make_unique<Workload>(std::move(processSequenceWithPrev))),
    updateCounter_(0)
{}

bool AudioEngineWorkerThreadPool::running() const
{
    return running_.test(std::memory_order_acquire);
}

void AudioEngineWorkerThreadPool::setThreadAffinities(const std::vector<std::uint16_t>& threadAffinities)
{

}

void AudioEngineWorkerThreadPool::start()
{
    if(!running())
    {
        running_.test_and_set(std::memory_order_release);
        running_.notify_all();
    }
}

void AudioEngineWorkerThreadPool::stop()
{
    if(running())
    {
        running_.clear(std::memory_order_release);
    }
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
