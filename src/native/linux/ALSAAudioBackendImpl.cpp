#if(__linux__)

#include "ALSAAudioBackendImpl.hpp"

#include "native/linux/ALSADeviceEnumerator.hpp"
#include "util/Base.hpp"
#include "util/SampleFormat.hpp"

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
    SND_PCM_ACCESS_RW_NONINTERLEAVED,
    SND_PCM_ACCESS_MMAP_INTERLEAVED,
    SND_PCM_ACCESS_RW_INTERLEAVED
};

using SampleFormat = std::tuple<double, YADAW::Util::DoubleRE,
    float, YADAW::Util::FloatRE,
    std::int32_t, YADAW::Util::Int32RE,
    std::int32_t, YADAW::Util::Int32RE,
    char[3], char[3],
    std::int16_t, YADAW::Util::Int16RE,
    std::int8_t>;

std::once_flag flag;

std::uint64_t keyFromDeviceSelector(YADAW::Audio::Backend::ALSADeviceSelector selector)
{
    return (static_cast<std::uint64_t>(selector.cIndex) << 32) + selector.dIndex;
}

namespace YADAW::Audio::Backend
{
ALSAAudioBackend::Impl::Impl(std::uint32_t sampleRate, std::uint32_t frameSize):
    sampleRate_(sampleRate), frameSize_(frameSize) {}

ALSAAudioBackend::Impl::~Impl()
{
    for(const auto& [key, tuple]: inputs_)
    {
        const auto& [pcm, channelCount, format, access] = tuple;
        snd_pcm_close(pcm);
    }
    for(const auto& [key, tuple]: outputs_)
    {
        const auto& [pcm, channelCount, format, access] = tuple;
        snd_pcm_close(pcm);
    }
}

std::uint32_t ALSAAudioBackend::Impl::audioInputCount()
{
    return YADAW::Native::ALSADeviceEnumerator::audioInputDeviceCount();
}

std::uint32_t ALSAAudioBackend::Impl::audioOutputCount()
{
    return YADAW::Native::ALSADeviceEnumerator::audioOutputDeviceCount();
}

std::optional<ALSADeviceSelector>
    ALSAAudioBackend::Impl::audioInputDeviceAt(std::uint32_t index)
{
    return YADAW::Native::ALSADeviceEnumerator::audioInputDeviceAt(index);
}

std::optional<ALSADeviceSelector>
    ALSAAudioBackend::Impl::audioOutputDeviceAt(std::uint32_t index)
{
    return YADAW::Native::ALSADeviceEnumerator::audioOutputDeviceAt(index);
}

ALSAAudioBackend::ActivateDeviceResult
ALSAAudioBackend::Impl::setAudioInputDeviceActivated(ALSADeviceSelector selector, bool activated)
{
    auto key = keyFromDeviceSelector(selector);
    if(activated)
    {
        if(inputs_.find(key) != inputs_.end())
        {
            return ActivateDeviceResult::AlreadyDone;
        }
        auto result = activateDevice(true, selector);
        const auto& [pcm, r1, r2, r3] = result;
        if(pcm)
        {
            inputs_.emplace(key, result);
            return ActivateDeviceResult::Success;
        }
        return ActivateDeviceResult::Failed;
    }
    if(auto it = inputs_.find(key); it == inputs_.end())
    {
        return ActivateDeviceResult::AlreadyDone;
    }
    else
    {
        const auto& [pcm, r1, r2, r3] = it->second;
        inputs_.erase(key);
        snd_pcm_close(pcm);
        return ActivateDeviceResult::Success;
    }
}

ALSAAudioBackend::ActivateDeviceResult
ALSAAudioBackend::Impl::setAudioOutputDeviceActivated(ALSADeviceSelector selector, bool activated)
{
    auto key = keyFromDeviceSelector(selector);
    if(activated)
    {
        if(outputs_.find(key) != outputs_.end())
        {
            return ActivateDeviceResult::AlreadyDone;
        }
        auto result = activateDevice(true, selector);
        const auto& [pcm, r1, r2, r3] = result;
        if(pcm)
        {
            outputs_.emplace(key, result);
            return ActivateDeviceResult::Success;
        }
        return ActivateDeviceResult::Failed;
    }
    else
    {
        if(auto it = outputs_.find(key); it == outputs_.end())
        {
            return ActivateDeviceResult::AlreadyDone;
        }
        else
        {
            const auto& [pcm, r1, r2, r3] = it->second;
            inputs_.erase(key);
            snd_pcm_close(pcm);
            return ActivateDeviceResult::Success;
        }
    }
}

bool ALSAAudioBackend::Impl::isAudioInputDeviceActivated(ALSADeviceSelector selector) const
{
    auto key = keyFromDeviceSelector(selector);
    return inputs_.find(key) != inputs_.end();
}

bool ALSAAudioBackend::Impl::isAudioOutputDeviceActivated(ALSADeviceSelector selector) const
{
    auto key = keyFromDeviceSelector(selector);
    return outputs_.find(key) != outputs_.end();
}

bool ALSAAudioBackend::Impl::start()
{
    return false;
}

bool ALSAAudioBackend::Impl::stop()
{
    return false;
}

std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t>
    ALSAAudioBackend::Impl::activateDevice(bool isInput, ALSADeviceSelector selector)
{
    char name[26]; // hw:[0-9]+,[0-9]+
    auto count = std::sprintf(name, "hw:%d,%d", selector.cIndex, selector.dIndex);
    name[count] = 0;
    snd_pcm_t* pcm = nullptr;
    if(auto err = snd_pcm_open(&pcm, name,
        isInput? SND_PCM_STREAM_CAPTURE: SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
        err < 0)
    {
        return {};
    }
    snd_pcm_hw_params_t* hwParams = nullptr;
    snd_pcm_hw_params_alloca(&hwParams);
    snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
    for(int i = 0; i < YADAW::Util::stackArraySize(formats); ++i)
    {
        if(snd_pcm_hw_params_set_format(pcm, hwParams, formats[i]) == 0)
        {
            format = formats[i];
            break;
        }
    }
    if(format == SND_PCM_FORMAT_UNKNOWN)
    {
        snd_pcm_close(pcm);
        return {};
    }
    unsigned int channelCount = 0;
    if(auto err = snd_pcm_hw_params_set_rate(pcm, hwParams, sampleRate_, 0);
        err < 0)
    {
        snd_pcm_close(pcm);
        return {};
    }
    if(auto err = snd_pcm_hw_params_set_channels_max(pcm, hwParams, &channelCount);
        err < 0)
    {
        snd_pcm_close(pcm);
        return {};
    }
    int access = SND_PCM_ACCESS_LAST + 1;
    for(int i = 0; i < YADAW::Util::stackArraySize(accesses); ++i)
    {
        if(snd_pcm_hw_params_set_access(pcm, hwParams, accesses[i]) == 0)
        {
            access = accesses[i];
        }
    }
    if(access > SND_PCM_ACCESS_LAST)
    {
        snd_pcm_close(pcm);
        return {};
    }
    snd_pcm_access_mask_t* mask;
    snd_pcm_access_mask_alloca(&mask);
    snd_pcm_hw_params_any(pcm, hwParams);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_RW_NONINTERLEAVED);
    snd_pcm_access_mask_set(mask, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_access_mask(pcm, hwParams, mask);
    if(auto err = snd_pcm_hw_params_set_buffer_size(pcm, hwParams, frameSize_);
        err < 0)
    {
        snd_pcm_close(pcm);
        return {};
    }
    if(auto err = snd_pcm_hw_params(pcm, hwParams); err < 0)
    {
        snd_pcm_close(pcm);
        return {};
    }
    snd_pcm_sw_params_t* swParams;
    snd_pcm_sw_params_alloca(&swParams);
    snd_pcm_sw_params_current(pcm, swParams);
    snd_pcm_sw_params_set_avail_min(pcm, swParams, frameSize_);
    snd_pcm_sw_params_set_start_threshold(pcm, swParams, 0U);
    if(auto err = snd_pcm_sw_params(pcm, swParams); err < 0)
    {
        snd_pcm_close(pcm);
        return {};
    }
    return {pcm, channelCount, format, static_cast<snd_pcm_access_t>(access)};
}
}

#endif