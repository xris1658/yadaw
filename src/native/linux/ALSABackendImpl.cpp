#if __linux__

#include "ALSABackendImpl.hpp"

#include "native/linux/ALSADeviceEnumerator.hpp"
#include "util/Base.hpp"
#include "util/IntegerRange.hpp"
#include "util/SampleFormat.hpp"

#include <alloca.h>

#include <filesystem>
#include <mutex>

constexpr auto SND_PCM_FORMAT_FLOAT64_RE = SND_PCM_FORMAT_FLOAT64 == SND_PCM_FORMAT_FLOAT64_LE? SND_PCM_FORMAT_FLOAT64_BE: SND_PCM_FORMAT_FLOAT64_LE;
constexpr auto SND_PCM_FORMAT_FLOAT_RE = SND_PCM_FORMAT_FLOAT == SND_PCM_FORMAT_FLOAT_LE? SND_PCM_FORMAT_FLOAT_BE: SND_PCM_FORMAT_FLOAT_LE;
constexpr auto SND_PCM_FORMAT_S32_RE = SND_PCM_FORMAT_S32 == SND_PCM_FORMAT_S32_LE? SND_PCM_FORMAT_S32_BE: SND_PCM_FORMAT_S32_LE;
constexpr auto SND_PCM_FORMAT_U32_RE = SND_PCM_FORMAT_U32 == SND_PCM_FORMAT_U32_LE? SND_PCM_FORMAT_U32_BE: SND_PCM_FORMAT_U32_LE;
constexpr auto SND_PCM_FORMAT_S24_RE = SND_PCM_FORMAT_S24 == SND_PCM_FORMAT_S24_LE? SND_PCM_FORMAT_S24_BE: SND_PCM_FORMAT_S24_LE;
constexpr auto SND_PCM_FORMAT_U24_RE = SND_PCM_FORMAT_U24 == SND_PCM_FORMAT_U24_LE? SND_PCM_FORMAT_U24_BE: SND_PCM_FORMAT_U24_LE;
constexpr auto SND_PCM_FORMAT_S20_RE = SND_PCM_FORMAT_S20 == SND_PCM_FORMAT_S20_LE? SND_PCM_FORMAT_S20_BE: SND_PCM_FORMAT_S20_LE;
constexpr auto SND_PCM_FORMAT_U20_RE = SND_PCM_FORMAT_U20 == SND_PCM_FORMAT_U20_LE? SND_PCM_FORMAT_U20_BE: SND_PCM_FORMAT_U20_LE;
constexpr auto SND_PCM_FORMAT_S16_RE = SND_PCM_FORMAT_S16 == SND_PCM_FORMAT_S16_LE? SND_PCM_FORMAT_S16_BE: SND_PCM_FORMAT_S16_LE;
constexpr auto SND_PCM_FORMAT_U16_RE = SND_PCM_FORMAT_U16 == SND_PCM_FORMAT_U16_LE? SND_PCM_FORMAT_U16_BE: SND_PCM_FORMAT_U16_LE;

constexpr snd_pcm_format_t formats[] = {
    SND_PCM_FORMAT_FLOAT64,
    SND_PCM_FORMAT_FLOAT64_RE,
    SND_PCM_FORMAT_FLOAT,
    SND_PCM_FORMAT_FLOAT_RE,
    SND_PCM_FORMAT_S32,
    SND_PCM_FORMAT_S32_RE,
    SND_PCM_FORMAT_S24,
    SND_PCM_FORMAT_S24_RE,
    SND_PCM_FORMAT_S20,
    SND_PCM_FORMAT_S20_RE,
    SND_PCM_FORMAT_S16,
    SND_PCM_FORMAT_S16_RE,
    SND_PCM_FORMAT_S8
};

constexpr snd_pcm_access_t accesses[] = {
    SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
    SND_PCM_ACCESS_MMAP_INTERLEAVED,
    SND_PCM_ACCESS_RW_NONINTERLEAVED,
    SND_PCM_ACCESS_RW_INTERLEAVED
};

snd_pcm_sframes_t operator ""_SF(unsigned long long int value)
{
    return static_cast<snd_pcm_sframes_t>(value);
}

snd_pcm_uframes_t operator ""_UF(unsigned long long int value)
{
    return static_cast<snd_pcm_uframes_t>(value);
}

std::once_flag flag;

namespace YADAW::Audio::Backend
{
ALSABackend::SampleFormat fromPCMSampleFormat(snd_pcm_format_t format)
{
    switch(format)
    {
    case SND_PCM_FORMAT_FLOAT64:
        return ALSABackend::SampleFormat::DoubleFloat;
    case SND_PCM_FORMAT_FLOAT64_RE:
        return ALSABackend::SampleFormat::DoubleFloatRE;
    case SND_PCM_FORMAT_FLOAT:
        return ALSABackend::SampleFormat::Float;
    case SND_PCM_FORMAT_FLOAT_RE:
        return ALSABackend::SampleFormat::FloatRE;
    case SND_PCM_FORMAT_S32:
        return ALSABackend::SampleFormat::Int32;
    case SND_PCM_FORMAT_S32_RE:
        return ALSABackend::SampleFormat::Int32RE;
    case SND_PCM_FORMAT_S24:
        return ALSABackend::SampleFormat::Int24;
    case SND_PCM_FORMAT_S24_RE:
        return ALSABackend::SampleFormat::Int24RE;
    case SND_PCM_FORMAT_S20:
        return ALSABackend::SampleFormat::Int20;
    case SND_PCM_FORMAT_S20_RE:
        return ALSABackend::SampleFormat::Int20RE;
    case SND_PCM_FORMAT_S16:
        return ALSABackend::SampleFormat::Int16;
    case SND_PCM_FORMAT_S16_RE:
        return ALSABackend::SampleFormat::Int16RE;
    case SND_PCM_FORMAT_S8:
        return ALSABackend::SampleFormat::Int8;
    default:
        return ALSABackend::SampleFormat::Unknown;
    }
}

snd_pcm_sframes_t ALSABackend::Impl::readMMapInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    const snd_pcm_channel_area_t* channelAreas = nullptr;
    snd_pcm_mmap_begin(pcm, &channelAreas, &offset, &frames);
    if(frames >= frameCount)
    {
        auto& channelArea = channelAreas[0];
        buffer = static_cast<std::byte*>(channelArea.addr) + (channelArea.first >> 3) + offset * (channelArea.step >> 3);
        return snd_pcm_mmap_readi(pcm, buffer, frameCount);
    }
    return 0_SF;
}

snd_pcm_sframes_t ALSABackend::Impl::readMMapInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    return snd_pcm_mmap_commit(pcm, offset, frameCount);
}

snd_pcm_sframes_t ALSABackend::Impl::readMMapNonInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    const snd_pcm_channel_area_t* channelAreas = nullptr;
    snd_pcm_mmap_begin(pcm, &channelAreas, &offset, &frames);
    FOR_RANGE0(i, channelCount)
    {
        auto& channelArea = channelAreas[i];
        nonInterleaveArray[i] =
            static_cast<std::byte*>(channelArea.addr) + (channelArea.first >> 3) + offset * (channelArea.step >> 3);
    }
    auto ret = snd_pcm_mmap_readn(pcm, nonInterleaveArray, frameCount);
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::readMMapNonInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    return snd_pcm_mmap_commit(pcm, offset, frameCount);
}

snd_pcm_sframes_t ALSABackend::Impl::readInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_avail_update(pcm);
    if(ret >= frameCount)
    {
        ret = snd_pcm_readi(pcm, buffer, frameCount);
    }
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::readInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    return frameCount;
}

snd_pcm_sframes_t ALSABackend::Impl::readNonInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_readn(pcm, nonInterleaveArray, frameCount);
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::readNonInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    return frameCount;
}

snd_pcm_sframes_t ALSABackend::Impl::writeMMapInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    const snd_pcm_channel_area_t* channelArea = nullptr;
    auto ret = snd_pcm_mmap_begin(pcm, &channelArea, &offset, &frames);
    if(ret < 0)
    {
        return ret;
    }
    buffer = static_cast<std::byte*>(channelArea->addr) + (channelArea->first >> 3) + offset * (channelArea->step >> 3);
    return frameCount;
}

snd_pcm_sframes_t ALSABackend::Impl::writeMMapInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_mmap_writei(pcm, buffer, frameCount);
    if(ret < 0)
    {
        return ret;
    }
    ret = snd_pcm_mmap_commit(pcm, offset, frameCount);
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::writeMMapNonInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_avail_update(pcm);
    if(ret >= frameCount)
    {
        const snd_pcm_channel_area_t* channelAreas = nullptr;
        snd_pcm_mmap_begin(pcm, &channelAreas, &offset, &frames);
        FOR_RANGE0(i, channelCount)
        {
            auto& channelArea = channelAreas[i];
            nonInterleaveArray[i] =
                static_cast<std::byte*>(channelArea.addr)
                + (channelArea.first >> 3)
                + offset * (channelArea.step >> 3);
        }
        ret = frameCount;
    }
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::writeMMapNonInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_mmap_writen(pcm, nonInterleaveArray, frameCount);
    auto ret2 = snd_pcm_mmap_commit(pcm, offset, frameCount);
    if(ret2 < 0)
    {
        ret = ret2;
    }
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::writeInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_avail_update(pcm);
    if(ret >= frameCount)
    {
        return frameCount;
    }
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::writeInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    return snd_pcm_writei(pcm, buffer, frameCount);
}

snd_pcm_sframes_t ALSABackend::Impl::writeNonInterleavedBegin(
    std::uint32_t frameCount, DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    auto ret = snd_pcm_avail_update(pcm);
    if(ret >= frameCount)
    {
        return frameCount;
    }
    return ret;
}

snd_pcm_sframes_t ALSABackend::Impl::writeNonInterleavedEnd(
    std::uint32_t frameCount, ALSABackend::Impl::DataType& data)
{
    auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
    return snd_pcm_writen(pcm, nonInterleaveArray, frameCount);
}

ALSABackend::Impl::Impl(ALSABackend* backend):
    backend_(backend)
{}

ALSABackend::Impl::~Impl() {}

ALSABackend::ActivateDeviceResult ALSABackend::Impl::setAudioDeviceActivated(
    bool isInput, std::uint32_t index, bool activated)
{
    auto& container = isInput? inputs_: outputs_;
    if(index < container.size())
    {
        auto it = container.begin() + index;
        if(activated == (std::get<TupleElementType::PCMHandle>(*it) != nullptr))
        {
            return ActivateDeviceResult::AlreadyDone;
        }
        if(activated)
        {
            auto selector = std::get<TupleElementType::DeviceSelector>(*it);
            auto result = activateDevice(isInput, selector);
            const auto& [pcm, r1, r2, r3, buffer, nonInterleaveArray] = result;
            if(pcm)
            {
                *it = std::tuple_cat(
                    std::make_tuple(selector),
                    result,
                    std::make_tuple(0_UF, 0_UF)
                );
                return ActivateDeviceResult::Success;
            }
        }
        else
        {
            auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = *it;
            buffers_.erase(buffers_.find<std::byte*>(buffer));
            nonInterleaveArrays_.erase(nonInterleaveArrays_.find<void**>(nonInterleaveArray));
            snd_pcm_close(pcm);
            pcm = nullptr;
            return ActivateDeviceResult::Success;
        }
    }
    return ActivateDeviceResult::Failed;
}

void ALSABackend::Impl::initialize(std::uint32_t sampleRate, std::uint32_t frameCount)
{
    sampleRate_ = sampleRate;
    frameCount_ = frameCount;
    const auto& inputDevices = YADAW::Native::ALSADeviceEnumerator::audioInputDevices();
    inputs_.reserve(inputDevices.size());
    for(const auto& inputDevice: inputDevices)
    {
        inputs_.emplace_back(
            inputDevice, nullptr, 0U, snd_pcm_format_t {}, snd_pcm_access_t {}, nullptr, nullptr, 0_UF, 0_UF
        );
    }
    const auto& outputDevices = YADAW::Native::ALSADeviceEnumerator::audioOutputDevices();
    outputs_.reserve(outputDevices.size());
    for(const auto& outputDevice: outputDevices)
    {
        outputs_.emplace_back(
            outputDevice, nullptr, 0U, snd_pcm_format_t {}, snd_pcm_access_t {}, nullptr, nullptr, 0_UF, 0_UF
        );
    }
}

void ALSABackend::Impl::uninitialize()
{
    stop();
    for(const auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames]: inputs_)
    {
        if(pcm)
        {
            snd_pcm_close(pcm);
        }
    }
    for(const auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames]: outputs_)
    {
        if(pcm)
        {
            snd_pcm_close(pcm);
        }
    }
    sampleRate_ = 0U;
    frameCount_ = 0U;
}

std::uint32_t ALSABackend::Impl::sampleRate() const
{
    return sampleRate_;
}

std::uint32_t ALSABackend::Impl::frameCount() const
{
    return frameCount_;
}

std::uint32_t ALSABackend::Impl::audioInputCount()
{
    return YADAW::Native::ALSADeviceEnumerator::audioInputDevices().size();
}

std::uint32_t ALSABackend::Impl::audioOutputCount()
{
    return YADAW::Native::ALSADeviceEnumerator::audioOutputDevices().size();
}

std::optional<ALSADeviceSelector>
    ALSABackend::Impl::audioInputDeviceAt(std::uint32_t index)
{
    const auto& inputs = YADAW::Native::ALSADeviceEnumerator::audioInputDevices();
    return index < inputs.size()?
        std::optional(inputs[index]):
        std::nullopt;
}

std::optional<ALSADeviceSelector>
    ALSABackend::Impl::audioOutputDeviceAt(std::uint32_t index)
{
    const auto& outputs = YADAW::Native::ALSADeviceEnumerator::audioOutputDevices();
    return index < outputs.size()?
           std::optional(outputs[index]):
           std::nullopt;
}

std::optional<std::string> ALSABackend::Impl::audioDeviceName(ALSADeviceSelector selector)
{
    return YADAW::Native::ALSADeviceEnumerator::audioDeviceName(selector);
}

std::optional<std::string> ALSABackend::Impl::cardName(int cardIndex)
{
    return YADAW::Native::ALSADeviceEnumerator::cardName(cardIndex);
}

bool ALSABackend::Impl::isAudioDeviceActivated(bool isInput, std::uint32_t index) const
{
    return false;
}

std::uint32_t ALSABackend::Impl::channelCount(bool isInput, std::uint32_t index) const
{
    auto& container = isInput? inputs_: outputs_;
    if(index < container.size())
    {
        auto it = container.begin() + index;
        return std::get<TupleElementType::ChannelCount>(*it);
    }
    return 0;
}

bool ALSABackend::Impl::start(const ALSABackend::Callback* callback)
{
    static RWFunc* writeBegin[] = {
        &writeMMapInterleavedBegin,
        &writeMMapNonInterleavedBegin,
        nullptr,
        &writeInterleavedBegin,
        &writeNonInterleavedBegin
    };
    static RWFunc* writeEnd[] = {
        &writeMMapInterleavedEnd,
        &writeMMapNonInterleavedEnd,
        nullptr,
        &writeInterleavedEnd,
        &writeNonInterleavedEnd
    };
    static RWFunc* readBegin[] = {
        &readMMapInterleavedBegin,
        &readMMapNonInterleavedBegin,
        nullptr,
        &readInterleavedBegin,
        &readNonInterleavedBegin
    };
    static RWFunc* readEnd[] = {
        &readMMapInterleavedEnd,
        &readMMapNonInterleavedEnd,
        nullptr,
        &readInterleavedEnd,
        &readNonInterleavedEnd
    };
    if(!runFlag_.test_and_set(std::memory_order::memory_order_acquire))
    {
        std::vector<ALSABackend::AudioBuffer> inputBuffers;
        std::vector<ALSABackend::AudioBuffer> outputBuffers;
        inputBuffers.reserve(inputs_.size());
        outputBuffers.reserve(outputs_.size());
        for(auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames]: inputs_)
        {
            auto& inputBuffer = inputBuffers.emplace_back();
            inputBuffer.format = fromPCMSampleFormat(format);
            inputBuffer.interleaved = (
                access == SND_PCM_ACCESS_MMAP_INTERLEAVED
                || access == SND_PCM_ACCESS_RW_INTERLEAVED
            );
            inputBuffer.channelCount = channelCount;
            inputBuffer.buffer.interleavedBuffer = nullptr;
        }
        for(auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames]: outputs_)
        {
            auto& outputBuffer = outputBuffers.emplace_back();
            outputBuffer.format = fromPCMSampleFormat(format);
            outputBuffer.interleaved = (
                access == SND_PCM_ACCESS_MMAP_INTERLEAVED
                || access == SND_PCM_ACCESS_RW_INTERLEAVED
            );
            outputBuffer.channelCount = channelCount;
            outputBuffer.buffer.interleavedBuffer = nullptr;
        }
        audioThread_ = std::thread(
            [this, callback, inputBuffers = std::move(inputBuffers), outputBuffers = std::move(outputBuffers)]() mutable
            {
                snd_pcm_status_t* status = nullptr;
                snd_pcm_status_alloca(&status);
                snd_output_t* output = nullptr;
                snd_output_stdio_attach(&output, stdout, 0);
                auto waitTimeInMillisecond = static_cast<int>(
                    std::ceil(
                        frameCount_ * 1000 / static_cast<float>(sampleRate_)
                    )
                );
                while(runFlag_.test_and_set(std::memory_order::memory_order_acquire))
                {
                    FOR_RANGE0(i, inputs_.size())
                    {
                        auto& data = inputs_[i];
                        auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
                        if(pcm)
                        {
                            auto waitRet = snd_pcm_wait(pcm, waitTimeInMillisecond);
                            if(waitRet == -EPIPE)
                            {
                                snd_pcm_prepare(pcm);
                            }
                            auto ret = 0_SF;
                            do
                            {
                                ret = readBegin[access](frameCount_, data);
                            }
                            while(ret >= frameCount_);
                            inputBuffers[i].buffer.interleavedBuffer = buffer? buffer: static_cast<void*>(nonInterleaveArray);
                        }
                    }
                    FOR_RANGE0(i, outputs_.size())
                    {
                        auto& data = outputs_[i];
                        auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
                        if(pcm)
                        {
                            do
                            {
                                auto waitRet = snd_pcm_wait(pcm, waitTimeInMillisecond);
                                if(waitRet == 1)
                                {
                                    break;
                                }
                                else if(waitRet == -EPIPE)
                                {
                                    snd_pcm_prepare(pcm);
                                }

                            }
                            while(true);
                            while(true)
                            {
                                auto ret = writeBegin[access](frameCount_, data);
                                if(ret == frameCount_)
                                {
                                    break;
                                }
                            }
                            outputBuffers[i].buffer.interleavedBuffer = buffer? buffer: static_cast<void*>(nonInterleaveArray);
                        }
                    }
                    callback(backend_, inputBuffers.size(), inputBuffers.data(), outputBuffers.size(), outputBuffers.data());
                    for(auto& data: inputs_)
                    {
                        auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
                        if(pcm)
                        {
                            auto ret = readEnd[access](frameCount_, data);
                            while(ret == 0 && errno == -EPIPE)
                            {
                                ret = snd_pcm_prepare(pcm);
                            }
                        }
                    }
                    for(auto& data: outputs_)
                    {
                        auto& [selector, pcm, channelCount, format, access, buffer, nonInterleaveArray, offset, frames] = data;
                        if(pcm)
                        {
                            auto ret = writeEnd[access](frameCount_, data);
                            while(ret == 0 && errno == -EPIPE)
                            {
                                ret = snd_pcm_prepare(pcm);
                            }
                            // std::printf("writeEnd returned %ld\n", ret);
                        }
                    }
                }
                runFlag_.clear(std::memory_order::memory_order_release);
            }
        );
        auto nativeHandle = audioThread_.native_handle();
        pthread_setname_np(nativeHandle, "ALSABackend::Impl audio thread");
        return true;
    }
    return false;
}

bool ALSABackend::Impl::stop()
{
    if(runFlag_.test_and_set(std::memory_order::memory_order_acquire))
    {
        runFlag_.clear(std::memory_order::memory_order_release);
        audioThread_.join();
        return true;
    }
    runFlag_.clear(std::memory_order::memory_order_release);
    return false;
}

std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t, std::byte*, void**>
    ALSABackend::Impl::activateDevice(bool isInput, ALSADeviceSelector selector)
{
    char name[26]; // hw:[0-9]+,[0-9]+
    auto count = std::sprintf(name, "hw:%d,%d", selector.cIndex, selector.dIndex);
    name[count] = 0;
    snd_pcm_t* pcm = nullptr;
    int errorCode = snd_pcm_open(&pcm, name,
        isInput? SND_PCM_STREAM_CAPTURE: SND_PCM_STREAM_PLAYBACK,
        SND_PCM_NONBLOCK);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_open` failed with error code %d: %s\n",
            errorCode, errorMessage);
        return {};
    }
    snd_pcm_hw_params_t* hwParams = nullptr;
    snd_pcm_hw_params_alloca(&hwParams);
    errorCode = snd_pcm_hw_params_any(pcm, hwParams);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_hw_params_any` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
    FOR_RANGE0(i, YADAW::Util::stackArraySize(formats))
    {
        if(snd_pcm_hw_params_set_format(pcm, hwParams, formats[i]) == 0)
        {
            format = formats[i];
            break;
        }
    }
    if(format == SND_PCM_FORMAT_UNKNOWN)
    {
        std::fprintf(stderr, "`snd_pcm_hw_params_set_format` failed: unsupported sample format\n");
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_hw_params_set_rate(pcm, hwParams, sampleRate_, 0);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_hw_params_set_rate` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_hw_params_set_buffer_size(pcm, hwParams, frameCount_);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_hw_params_set_buffer_size` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    unsigned int channelCount = 0;
    errorCode = snd_pcm_hw_params_get_channels_max(hwParams, &channelCount);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_hw_params_get_channels_max` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_hw_params_set_channels(pcm, hwParams, channelCount);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_hw_params_set_channels` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    int access = SND_PCM_ACCESS_LAST + 1;
    FOR_RANGE0(i, YADAW::Util::stackArraySize(accesses))
    {
        if(snd_pcm_hw_params_set_access(pcm, hwParams, accesses[i]) == 0)
        {
            access = accesses[i];
            break;
        }
    }
    if(access > SND_PCM_ACCESS_LAST)
    {
        std::fprintf(stderr, "`snd_pcm_hw_params_set_access` failed: unsupported access\n");
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_hw_params(pcm, hwParams);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_hw_params` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    snd_pcm_sw_params_t* swParams;
    snd_pcm_sw_params_alloca(&swParams);
    errorCode = snd_pcm_sw_params_current(pcm, swParams);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_sw_params_current` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_sw_params_set_avail_min(pcm, swParams, frameCount_);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_sw_params_set_avail_min` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_sw_params_set_start_threshold(pcm, swParams, 0U);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_sw_params_set_start_threshold` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    errorCode = snd_pcm_sw_params(pcm, swParams);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_sw_params` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    std::byte* ptr = nullptr;
    if(access > SND_PCM_ACCESS_MMAP_COMPLEX)
    {
        auto buffer = allocateBuffer(frameCount_, channelCount, format);
        if(!buffer)
        {
            std::fprintf(stderr, "Allocate buffer failed\n");
            snd_pcm_close(pcm);
            return {};
        }
        ptr = buffer.get();
        buffers_.emplace(std::move(buffer));
    }
    void** nonInterleaveArray = nullptr;
    if(access == SND_PCM_ACCESS_MMAP_NONINTERLEAVED
        || access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
    {
        nonInterleaveArray = static_cast<void**>(
            ::operator new[](
                sizeof(void*) * channelCount,
                std::align_val_t(sizeof(void*)),
                std::nothrow
            )
        );
        if(nonInterleaveArray)
        {
            if(access == SND_PCM_ACCESS_RW_NONINTERLEAVED)
            {
                auto singleBufferSize = frameCount_ * snd_pcm_format_physical_width(format);
                auto nonInterleaveBuffer = ptr;
                FOR_RANGE0(i, channelCount)
                {
                    nonInterleaveArray[i] = nonInterleaveBuffer;
                    nonInterleaveBuffer += singleBufferSize;
                }
            }
            auto deleter = [](void* ptr)
            {
                ::operator delete[](ptr, std::align_val_t(sizeof(void*)));
            };
            std::shared_ptr<void*[]> sharedNonInterleaveArray(
                nonInterleaveArray, deleter);
            nonInterleaveArrays_.emplace(std::move(nonInterleaveArray));
        }
        else
        {
            std::fprintf(stderr, "Allocate non-interleave array failed\n");
            snd_pcm_close(pcm);
            return {};
        }
    }
    return {pcm, channelCount, format, static_cast<snd_pcm_access_t>(access), ptr, nonInterleaveArray};
}

std::shared_ptr<std::byte[]>
ALSABackend::Impl::allocateBuffer(std::uint32_t frameCount, std::uint32_t channelCount, snd_pcm_format_t format)
{
    auto physicalWidth = snd_pcm_format_physical_width(format);
    auto ptr = static_cast<std::byte*>(
        ::operator new[](
            frameCount * channelCount * physicalWidth,
            std::align_val_t(physicalWidth), std::nothrow
        )
    );
    if(ptr)
    {
        auto deleter = [align = std::align_val_t(physicalWidth)](void* ptr)
        {
            ::operator delete[](ptr, align);
        };
        std::shared_ptr<std::byte[]> buffer(ptr, deleter);
        return buffer;
    }
    return nullptr;
}
}

#endif