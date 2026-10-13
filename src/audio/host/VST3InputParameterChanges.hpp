#ifndef YADAW_SRC_AUDIO_HOST_VST3INPUTPARAMETERCHANGES
#define YADAW_SRC_AUDIO_HOST_VST3INPUTPARAMETERCHANGES

#include "concurrent/AtomicMutex.hpp"
#include "util/OptionalUtil.hpp"

#include <pluginterfaces/vst/ivstparameterchanges.h>

#include <plf_hive.h>

namespace YADAW::Audio::Host
{
class VST3InputParameterChanges: public Steinberg::Vst::IParameterChanges
{
public:
    // A "double-buffered" SPSC circular queue.
    // The UI thread produces data on the queue repeatedly by calling `addPoint`
    // while the realtime thread consumes data on the queue.
    // This class is designed with the following fact in mind that the audio
    // thread might process the audio buffer by splitting it up into multiple
    // "parts" (see description of `Steinberg::Vst::ProcessContext`), that is to
    // say the data produced by the UI thread will be consumed in the same way.
    // To do this, we have two functions consuming the data:
    // - `prepareQueueForAudioThreadAndConsume` calculates the range of points
    //   prepared for processing from within the current callback, and the range
    //   of the first "part" for the plugin to use. This is called first in the
    //   audio callback, and is the only function to call if the audio buffer is
    //   not split.
    // - `consumeNext` calculates the range of the next "part" for the plugin to
    //   use. This is called after we've done processing the previous "part".
    //   Also, this function marks the previous "part" has been read and used
    //   to free up space for the UI thread.
    // Sample code for the audio thread:
    // ```
    // std::uint16_t sampleCount;
    // std::vector<std::uint16_t> partsLengthInSampleCount;
    // std::uint64_t consumeSampleOffset;
    // if(partsLength.empty())
    // {
    //     prepareQueueForAudioThreadAndConsume(consumeSampleOffset, sampleCount);
    //     // calling plugin process
    // }
    // else
    // {
    //     prepareQueueForAudioThreadAndConsume(
    //         consumeSampleOffset,
    //         sampleCount,
    //         partsLengthInSampleCount.front()
    //     );
    //     // calling plugin process
    //     for(auto it = partsLengthInSampleCount.begin() + 1;
    //         it != partsLengthInSampleCount.end();
    //         ++it)
    //     {
    //         consumeNext(*it);
    //         // calling plugin process
    //     }
    //     endConsume();
    // }
    // ```
    // - Latency: since data in the queue is always read in the "next" audio
    //   callback, the latency between _a_point_is_added_by_calling_`addPoint`_
    //   and _that_point_"affects"_the_audio_signal_ is always one `bufferSize`.
    class ParamValueQueue: public Steinberg::Vst::IParamValueQueue
    {
    private:
        struct Point
        {
            std::uint64_t sampleOffset;
            Steinberg::Vst::ParamValue value;
            friend bool operator<(const Point& lhs, std::uint64_t rhs) { return lhs.sampleOffset < rhs; }
        };
    public:
        ParamValueQueue(Steinberg::Vst::ParamID paramID, std::uint16_t bufferSize);
        ~ParamValueQueue();
    public:
        std::uint16_t latencyInSamples() const noexcept
        {
            return (capacity_ - 1) / 2;
        }
        static std::uint64_t capacityFromLatency(std::uint32_t latency)
        {
            return latency * 2 + 1;
        }
        bool addPoint(std::uint64_t sampleOffset, Steinberg::Vst::ParamValue value);
        OptionalRef<const Point> getPoint(std::uint16_t index) const;
        OptionalRef<Point> getPoint(std::uint16_t index);
        void prepareQueueForAudioThreadAndConsume(
            std::uint64_t consumeSampleOffset,
            std::uint16_t consumeSampleCount = 0
        );
        void consumeNext(std::uint16_t consumeSampleCount = 0);
        void endConsuming();
    public:
        Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void** obj) override;
        Steinberg::uint32 addRef() override;
        Steinberg::uint32 release() override;
        Steinberg::Vst::ParamID getParameterId() override;
        Steinberg::int32 getPointCount() override;
        Steinberg::tresult getPoint(Steinberg::int32 index, Steinberg::int32& sampleOffset, Steinberg::Vst::ParamValue& value) override;
        Steinberg::tresult addPoint(Steinberg::int32 sampleOffset, Steinberg::Vst::ParamValue value, Steinberg::int32& index) override;
    private:
        std::unique_ptr<Point[]> points_;
        std::atomic<std::uint32_t> beginAndMiddle_ = 0; // read in UI thread, write in audio thread
        std::atomic<std::uint16_t> end_ = 0; // write in UI thread, read in audio thread
        std::uint16_t capacity_; // init in ctor, always read
        std::uint16_t consumerNextBegin_ = 0; // always read/write in audio thread
        Steinberg::Vst::ParamID paramID_;
        std::uint64_t consumeSampleOffset_ = 0; // always read/write in audio thread
        std::uint64_t consumeNextSampleOffset_ = 0; // always read/write in audio thread
        std::uint64_t consumeEndSampleOffset_ = 0; // always read/write in audio thread
    };
public:
    VST3InputParameterChanges(std::uint64_t sampleOffset, std::uint16_t singleBufferSize);
    ~VST3InputParameterChanges();
public:
    void reserveParamCount(std::uint32_t count);
    std::pair<int, ParamValueQueue*> emplaceParameterData(Steinberg::Vst::ParamID id);
    void prepareQueueForAudioThreadAndConsume(
        std::uint64_t consumeSampleOffset,
        std::uint16_t consumeSampleCount = 0
    );
    void consumeNext(std::uint16_t consumeSampleCount = 0);
    void endConsuming();
public:
    Steinberg::tresult queryInterface(const Steinberg::TUID _iid, void** obj) override;
    Steinberg::uint32 addRef() override;
    Steinberg::uint32 release() override;
    Steinberg::int32 getParameterCount() override;
    YADAW::Audio::Host::VST3InputParameterChanges::ParamValueQueue* getParameterData(Steinberg::int32 index) override;
    YADAW::Audio::Host::VST3InputParameterChanges::ParamValueQueue* addParameterData(const Steinberg::Vst::ParamID& id, Steinberg::int32& index) override;
private:
    static plf::hive<ParamValueQueue> paramValueQueuePool_;
    std::vector<ParamValueQueue*> paramValueQueues_;
    ParamValueQueue** doNotConsumeNextFromHere_ = nullptr;
    YADAW::Concurrent::AtomicMutex accessingParamValueQueue_;
    std::uint64_t sampleOffset_;
    std::uint16_t singleBufferSize_;
};
}

#endif // YADAW_SRC_AUDIO_HOST_VST3INPUTPARAMETERCHANGES
