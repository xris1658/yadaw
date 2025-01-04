#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIOENGINEWORKERTHREADPOOL
#define YADAW_SRC_AUDIO_ENGINE_AUDIOENGINEWORKERTHREADPOOL

#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "concurrent/PassDataToRealtimeThread.hpp"
#include "util/VectorTypes.hpp"

#include <atomic>
#include <type_traits>
#include <iterator>

namespace YADAW::Audio::Engine
{
// Facility for running audio process tasks in a multi-threaded manner.
class AudioEngineWorkerThreadPool
{
public:
    struct Workload
    {
        Workload(std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev);
        ~Workload();
        std::unique_ptr<ProcessSequenceWithPrev> processSequenceWithPrev;
        std::vector<AudioThreadWorkload> audioThreadWorkload;
        Vec<Vec<std::uint8_t>> completionMarks;
        Vec<Vec<std::atomic_ref<std::uint8_t>>> atomicCompletionMarks;
    };
public:
    AudioEngineWorkerThreadPool(std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev);
    ~AudioEngineWorkerThreadPool();
public:
    bool running() const;
    // TODO: Not all tasks of processing are done in the worker threads. We need
    //       to assign some tasks to the "main" audio thread, which is the one
    //       running the audio callback. For now I'm looking for a way to
    //       decouple this with the audio callback.
    void setThreadAffinities(const std::vector<std::uint16_t>& threadAffinities);
    void start();
    void stop();
private:
    void workerThreadFunc(std::uint32_t processorIndex, std::uint32_t workloadIndex);
    void workerFunc(std::uint32_t workloadIndex);
private:
    YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<ProcessSequenceWithPrev>> processSequenceWithPrev_;
    YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<Workload>> workload_;
    std::vector<std::thread> workerThreads_;
    std::atomic_uint16_t updateCounter_;
    std::atomic_flag running_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIOENGINEWORKERTHREADPOOL