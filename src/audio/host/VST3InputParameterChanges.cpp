#include "VST3InputParameterChanges.hpp"

#include "util/IntegerRange.hpp"

#include <cinttypes>
#include <mutex>

// #define VST3_PVQLOG
// #define VST3_IPCLOG

namespace YADAW::Audio::Host
{
plf::hive<YADAW::Audio::Host::VST3InputParameterChanges::ParamValueQueue>
    VST3InputParameterChanges::paramValueQueuePool_;

VST3InputParameterChanges::ParamValueQueue::ParamValueQueue(
    Steinberg::Vst::ParamID paramID, std::uint16_t bufferSize):
    points_(std::make_unique<Point[]>(capacityFromLatency(bufferSize))),
    capacity_(capacityFromLatency(bufferSize)),
    paramID_(paramID)
{}

VST3InputParameterChanges::ParamValueQueue::~ParamValueQueue()
{}

struct BeginAndMidddle
{
    std::uint16_t begin;
    std::uint16_t middle;
};

inline BeginAndMidddle beginAndMiddle(const std::atomic<std::uint32_t>& beginAndMiddle)
{
    auto bam = beginAndMiddle.load(std::memory_order_acquire);
    return BeginAndMidddle {
        .begin = static_cast<std::uint16_t>(bam & 0xFFFF),
        .middle = static_cast<std::uint16_t>(bam >> 16)
    };
}

inline void writeBeginAndMiddle(BeginAndMidddle bam, std::atomic<std::uint32_t>& dest)
{
    dest.store((bam.middle << 16) + bam.begin, std::memory_order_release);
}

bool VST3InputParameterChanges::ParamValueQueue::addPoint(
    std::uint64_t sampleOffset, Steinberg::Vst::ParamValue value)
{
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "ParamValueQueue::addPoint(%" PRIu64", %lf)\n", sampleOffset, value);
#endif
    auto [begin, middle] = beginAndMiddle(beginAndMiddle_);
    auto oldEnd = end_.load(std::memory_order_acquire);
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "begin, middle, end: % 4" PRIu16", % 4" PRIu16", % 4" PRIu16"\n", begin, middle, oldEnd);
#endif
    if(begin == 0? oldEnd != capacity_ - 1: oldEnd != begin - 1)
    {
        points_[oldEnd].sampleOffset = sampleOffset;
        points_[oldEnd].value = value;
        end_.store(oldEnd == capacity_ - 1? 0: oldEnd + 1, std::memory_order_release);
#ifdef VST3_PVQLOG
        std::fprintf(stderr, " Returns true\n");
#endif
        return true;
    }
#ifdef VST3_PVQLOG
        std::fprintf(stderr, " Returns false\n");
#endif
    return false;
}

OptionalRef<const VST3InputParameterChanges::ParamValueQueue::Point>
    VST3InputParameterChanges::ParamValueQueue::getPoint(std::uint16_t index) const
{
    auto bam = beginAndMiddle(beginAndMiddle_);
    auto pointCount = consumerNextBegin_ - bam.begin;
    if(index < pointCount)
    {
        auto pointIndex = bam.begin + index;
        if(pointIndex >= capacity_)
        {
            pointIndex -= capacity_;
        }
        return points_[pointIndex];
    }
    return std::nullopt;
}

OptionalRef<VST3InputParameterChanges::ParamValueQueue::Point>
    VST3InputParameterChanges::ParamValueQueue::getPoint(std::uint16_t index)
{
    auto bam = beginAndMiddle(beginAndMiddle_);
    auto pointCount = consumerNextBegin_ - bam.begin;
    if(index < pointCount)
    {
        auto pointIndex = bam.begin + index;
        if(pointIndex >= capacity_)
        {
            pointIndex -= capacity_;
        }
        return points_[pointIndex];
    }
    return std::nullopt;
}

void VST3InputParameterChanges::ParamValueQueue::prepareQueueForAudioThreadAndConsume(
    std::uint64_t consumeSampleOffset,
    std::uint16_t consumeSampleCount)
{
    consumeNextSampleOffset_ = consumeSampleOffset - latencyInSamples();
    consumeEndSampleOffset_ = consumeSampleOffset;
    auto bam = beginAndMiddle(beginAndMiddle_); auto& [begin, middle] = bam;
    auto end = end_.load(std::memory_order_acquire);
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "ParamValueQueue::prepareQueueForAudioThreadAndConsume(%" PRIu64", %" PRIu16")\n", consumeSampleOffset, consumeSampleCount);
    std::fprintf(stderr, "Sample offset (next, end): %" PRIu64", %" PRIu64"\n", consumeNextSampleOffset_, consumeEndSampleOffset_);
    // std::fprintf(stderr, "begin, middle, end: % 4" PRIu16", % 4" PRIu16", % 4" PRIu16"\n", begin, middle, end);
#endif
    if(middle < end)
    {
        auto newMiddle = std::lower_bound(
            points_.get() + middle,
            points_.get() + end,
            consumeEndSampleOffset_
        ) - points_.get();
        begin = middle; middle = newMiddle;
    }
    else
    {
        auto newMiddlePtr = std::lower_bound(
            points_.get() + middle, points_.get() + capacity_, consumeEndSampleOffset_
        );
        if(newMiddlePtr == points_.get() + capacity_)
        {
            newMiddlePtr = std::lower_bound(
                points_.get(), points_.get() + end, consumeEndSampleOffset_
            );
        }
        auto newMiddle = newMiddlePtr - points_.get();
        begin = middle; middle = newMiddle;
    }
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "begin, middle, end: % 4" PRIu16", % 4" PRIu16", % 4" PRIu16"\n", begin, middle, end);
#endif
    writeBeginAndMiddle(bam, beginAndMiddle_);
    consumeNext(consumeSampleCount);
}

void VST3InputParameterChanges::ParamValueQueue::consumeNext(std::uint16_t consumeSampleCount)
{
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "ParamValueQueue::consumeNext(%" PRIu16")\n", consumeSampleCount);
#endif
    consumeSampleOffset_ = consumeNextSampleOffset_;
    auto bam = beginAndMiddle(beginAndMiddle_);
    bam.begin = consumerNextBegin_;
    writeBeginAndMiddle(bam, beginAndMiddle_);
    if(consumeSampleCount == 0
    || consumeNextSampleOffset_ + consumeSampleCount > consumeEndSampleOffset_)
    {
        consumeNextSampleOffset_ = consumeEndSampleOffset_;
        consumerNextBegin_ = bam.middle;
    }
    else
    {
        consumeNextSampleOffset_ += consumeSampleCount;
        auto middle = bam.middle;
        if(consumerNextBegin_ <= middle)
        {
            consumerNextBegin_ = std::lower_bound(
                points_.get() + consumerNextBegin_,
                points_.get() + middle,
                consumeNextSampleOffset_
            ) - points_.get();
        }
        else
        {
            consumerNextBegin_ = std::lower_bound(
                points_.get() + consumerNextBegin_,
                points_.get() + capacity_,
                consumeNextSampleOffset_
            ) - points_.get();
            if(consumerNextBegin_ == capacity_)
            {
                consumerNextBegin_ = std::lower_bound(
                    points_.get(),
                    points_.get() + middle,
                    consumeNextSampleOffset_
                ) - points_.get();
            }
        }
    }
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "consuming sample count: %" PRIu16"\n", consumerNextBegin_ - bam.begin);
#endif
}

void VST3InputParameterChanges::ParamValueQueue::endConsuming()
{
    auto bam = beginAndMiddle(beginAndMiddle_);
    bam.begin = bam.middle;
    writeBeginAndMiddle(bam, beginAndMiddle_);
}

Steinberg::tresult VST3InputParameterChanges::ParamValueQueue::queryInterface(const Steinberg::TUID _iid, void** obj)
{
     QUERY_INTERFACE(_iid, obj, FUnknown::iid, IParamValueQueue)
     QUERY_INTERFACE(_iid, obj, IParamValueQueue::iid, IParamValueQueue)
     *obj = nullptr;
     return Steinberg::kNoInterface;
}

Steinberg::uint32 VST3InputParameterChanges::ParamValueQueue::addRef()
{
    return 1U;
}

Steinberg::uint32 VST3InputParameterChanges::ParamValueQueue::release()
{
    return 1U;
}

Steinberg::Vst::ParamID VST3InputParameterChanges::ParamValueQueue::getParameterId()
{
    return paramID_;
}

Steinberg::int32 VST3InputParameterChanges::ParamValueQueue::getPointCount()
{
    auto begin = beginAndMiddle(beginAndMiddle_).begin;
    return consumerNextBegin_ >= begin?
        consumerNextBegin_ - begin:
        consumerNextBegin_ + capacity_ - begin;
}

Steinberg::tresult VST3InputParameterChanges::ParamValueQueue::getPoint(
    Steinberg::int32 index, Steinberg::int32& sampleOffset, Steinberg::Vst::ParamValue& value)
{
    auto begin = beginAndMiddle(beginAndMiddle_).begin;
    auto pointCount = consumerNextBegin_ >= begin?
        consumerNextBegin_ - begin:
        consumerNextBegin_ + capacity_ - begin;
    if(index >= 0 && index < pointCount)
    {
        auto pointIndex = begin + index;
        if(pointIndex >= capacity_)
        {
            pointIndex -= capacity_;
        }
        auto& point = points_[pointIndex];
        sampleOffset = point.sampleOffset  - consumeSampleOffset_;
        value = point.value;
#ifdef VST3_PVQLOG
        std::fprintf(stderr, "Point #%" PRId32": %" PRId32" - %" PRId32", %lf\n", index, point.sampleOffset, consumeSampleOffset_, value);
#endif
        return Steinberg::kResultOk;
    }
    return Steinberg::kInvalidArgument;
}

Steinberg::tresult VST3InputParameterChanges::ParamValueQueue::addPoint(
    Steinberg::int32 sampleOffset, Steinberg::Vst::ParamValue value, Steinberg::int32& index)
{
#ifdef VST3_PVQLOG
    std::fprintf(stderr, "IParamValueQueue::addPoint returns kNotImplemented\n");
#endif
    return Steinberg::kNotImplemented;
}

VST3InputParameterChanges::VST3InputParameterChanges(
    std::uint64_t sampleOffset, std::uint16_t singleBufferSize):
    sampleOffset_(sampleOffset), singleBufferSize_(singleBufferSize)
{}

VST3InputParameterChanges::~VST3InputParameterChanges()
{
    for(auto paramValueQueue: paramValueQueues_)
    {
        paramValueQueuePool_.erase(
            paramValueQueuePool_.get_iterator(paramValueQueue)
        );
    }
}

void VST3InputParameterChanges::reserveParamCount(std::uint32_t count)
{
    paramValueQueues_.reserve(count);
}

std::pair<int, VST3InputParameterChanges::ParamValueQueue*>
VST3InputParameterChanges::emplaceParameterData(
    Steinberg::Vst::ParamID id)
{
    for(int i = 0; i < paramValueQueues_.size(); ++i)
    {
        if(auto ptr = paramValueQueues_[i]; ptr->getParameterId() == id)
        {
            return {i, ptr};
        }
    }
    if(auto lg = std::lock_guard(accessingParamValueQueue_);
        paramValueQueues_.size() < paramValueQueues_.capacity())
    {
        auto& ret = paramValueQueues_.emplace_back(
            &*paramValueQueuePool_.emplace(
                id, singleBufferSize_
            )
        );
        return {static_cast<int>(paramValueQueues_.size() - 1), ret};
    }
    return {-1, nullptr};
}

void VST3InputParameterChanges::prepareQueueForAudioThreadAndConsume(
    std::uint64_t consumeSampleOffset, std::uint16_t consumeSampleCount)
{
    {
        auto lg = std::lock_guard(accessingParamValueQueue_);
        consumeCount_ = paramValueQueues_.size();
    }
    for(std::size_t i = 0; i < consumeCount_; ++i)
    {
        paramValueQueues_[i]->prepareQueueForAudioThreadAndConsume(
            consumeSampleOffset, consumeSampleCount
        );
    }
}

void VST3InputParameterChanges::consumeNext(std::uint16_t consumeSampleCount)
{
    for(std::size_t i = 0; i < consumeCount_; ++i)
    {
        paramValueQueues_[i]->consumeNext(consumeSampleCount);
    }
}

void VST3InputParameterChanges::endConsuming()
{
    for(std::size_t i = 0; i < consumeCount_; ++i)
    {
        paramValueQueues_[i]->endConsuming();
    }
}

Steinberg::tresult VST3InputParameterChanges::queryInterface(const Steinberg::TUID _iid, void** obj)
{
    QUERY_INTERFACE(_iid, obj, FUnknown::iid, IParameterChanges)
    QUERY_INTERFACE(_iid, obj, IParameterChanges::iid, IParameterChanges)
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 VST3InputParameterChanges::addRef()
{
    return 1U;
}

Steinberg::uint32 VST3InputParameterChanges::release()
{
    return 1U;
}

Steinberg::int32 VST3InputParameterChanges::getParameterCount()
{
#ifdef VST3_IPCLOG
    std::fprintf(stderr, "VST3InputParameterChanges::getParameterCount()");
#endif
    auto lg = std::lock_guard(accessingParamValueQueue_);
    auto ret = static_cast<Steinberg::int32>(paramValueQueues_.size());
#ifdef VST3_IPCLOG
    std::fprintf(stderr, " returns %" PRId32"\n", ret);
#endif
    return ret;
}

YADAW::Audio::Host::VST3InputParameterChanges::ParamValueQueue* VST3InputParameterChanges::getParameterData(
    Steinberg::int32 index)
{
    auto lg = std::lock_guard(accessingParamValueQueue_);
    if(index < paramValueQueues_.size())
    {
        return paramValueQueues_[index];
    }
    return nullptr;
}

YADAW::Audio::Host::VST3InputParameterChanges::ParamValueQueue* VST3InputParameterChanges::addParameterData(
    const Steinberg::Vst::ParamID& id, Steinberg::int32& index)
{
    FOR_RANGE0(i, paramValueQueues_.size())
    {
        if(auto paramValueQueue = paramValueQueues_[i];
            paramValueQueue->getParameterId() == id)
        {
            index = i;
            return paramValueQueue;
        }
    }
    auto ret = paramValueQueues_.emplace_back(
        &*paramValueQueuePool_.emplace(
            id, singleBufferSize_
        )
    );
    index = paramValueQueues_.size() - 1;
    return ret;
}
}
