#ifndef YADAW_SRC_AUDIO_ENGINE_AUDIOENGINEWORKERTHREADPOOL
#define YADAW_SRC_AUDIO_ENGINE_AUDIOENGINEWORKERTHREADPOOL

#include "audio/engine/AudioDeviceGraphProcess.hpp"
#include "concurrent/PassDataToRealtimeThread.hpp"
#include "util/AlignHelper.hpp"
#include "util/VectorTypes.hpp"

#include <atomic>
#include <functional>
#include <thread>

namespace YADAW::Audio::Engine
{
// Facility for running audio process tasks in a multi-threaded manner.
class AudioEngineWorkerThreadPool
{
public:
    struct Workload
    {
        Workload(
            std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev,
            std::uint32_t threadCount
        );
        ~Workload();
        std::unique_ptr<ProcessSequenceWithPrev> processSequenceWithPrev;
        std::vector<AudioThreadWorkload> audioThreadWorkload;
        Vec<Vec<YADAW::Util::AlignedStorage<std::atomic_flag>>> completionMarksStorage;
    };
    enum WorkerThreadTaskStatus: std::uint8_t
    {
        NoWorker,
        ProcessedByAudioCallbackThread,
        ProcessedByWorkerThread
    };
    using SetAudioThreadIdCallback = void(const AudioEngineWorkerThreadPool& sender, std::thread::id id);
    using UnsetAudioThreadIdCallback = void(const AudioEngineWorkerThreadPool& sender, std::thread::id id);
public:
    AudioEngineWorkerThreadPool(std::unique_ptr<ProcessSequenceWithPrev>&&);
    ~AudioEngineWorkerThreadPool();
public:
    bool running() const;
    bool setAffinities(const std::vector<std::uint16_t>& affinities);
    std::uint32_t getMainAffinity() const;
    bool start();
    // The timing and calling thread of this call depends on how your audio
    // backend works. Calling `stop` in an inappropriate time point or thread
    // will cause an unreachable waiting situation.
    // Some audio backends hold the audio thread and users are only able to
    // customize the audio callback (e.g. AudioGraph on Windows), while some
    // backends do not create audio threads, letting users manage audio threads
    // and run status (e.g. ALSA on Linux).
    void stop();
    void mainFunc();
    void updateProcessSequence(
        std::unique_ptr<ProcessSequenceWithPrev>&& processSequenceWithPrev);
public:
    void setSetAudioThreadIdCallback(std::function<SetAudioThreadIdCallback>&& setAudioThreadIdCallback);
    void setUnsetAudioThreadIdCallback(std::function<UnsetAudioThreadIdCallback>&& unsetAudioThreadIdCallback);
    void resetSetAudioThreadIdCallback();
    void resetUnsetAudioThreadIdCallback();
private:
    void workerThreadFunc(std::uint32_t processorIndex, std::uint32_t workloadIndex);
    void workerFunc(std::uint32_t workloadIndex);
private:
    YADAW::Concurrent::PassDataToRealtimeThread<std::unique_ptr<Workload>> workload_;
    std::vector<std::thread> workerThreads_;
    std::vector<std::uint16_t> affinities_;
    std::vector<YADAW::Util::AlignedStorage<std::atomic_flag>> workerThreadDoneStorage_;
    std::atomic_flag firstCallback_;
    std::atomic_flag running_;
    mutable bool mainAffinityIsSet_ = false;
    std::function<SetAudioThreadIdCallback> setAudioThreadIdCallback_;
    std::function<UnsetAudioThreadIdCallback> unsetAudioThreadIdCallback_;
};
}

#endif // YADAW_SRC_AUDIO_ENGINE_AUDIOENGINEWORKERTHREADPOOL