#if __linux__

#include "ALSABackendImpl.hpp"

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
    SND_PCM_ACCESS_MMAP_INTERLEAVED,
    SND_PCM_ACCESS_RW_NONINTERLEAVED,
    SND_PCM_ACCESS_RW_INTERLEAVED
};

using SampleFormat = std::tuple<
    double, YADAW::Util::DoubleRE,
    float, YADAW::Util::FloatRE,
    std::int32_t, YADAW::Util::Int32RE,
    std::int32_t, YADAW::Util::Int32RE,
    std::int32_t, YADAW::Util::Int32RE,
    std::int16_t, YADAW::Util::Int16RE,
    std::int8_t>;

constexpr std::size_t sampleSize[] = {
    8, 8, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 1
};

std::once_flag flag;

bool operator<(const std::shared_ptr<std::byte[]>& lhs, std::byte* const rhs)
{
    return lhs.get() < rhs;
}

bool operator<(std::byte* const lhs, const std::shared_ptr<std::byte[]>& rhs)
{
    return lhs < rhs.get();
}

namespace YADAW::Audio::Backend
{
ALSABackend::Impl::Impl() {}

ALSABackend::Impl::~Impl() {}

bool ALSABackend::Impl::compareTupleWithElement(
    ALSABackend::Impl::ContainerType::const_reference elem,
    ALSADeviceSelector selector)
{
    return selector < std::get<TupleElementType::DeviceSelector>(elem);
}

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
            const auto& [pcm, r1, r2, r3, buffer] = result;
            if(pcm)
            {
                *it = std::tuple_cat(
                    std::make_tuple(selector),
                    result);
                return ActivateDeviceResult::Success;
            }
        }
        else
        {
            auto pcm = std::get<TupleElementType::PCMHandle>(*it);
            auto buffer = std::get<TupleElementType::Buffer>(*it);
            buffers_.erase(buffers_.find<std::byte*>(buffer));
            snd_pcm_close(pcm);
            pcm = nullptr;
            return ActivateDeviceResult::Success;
        }
    }
    return ActivateDeviceResult::Failed;
}

void ALSABackend::Impl::initialize(std::uint32_t sampleRate, std::uint32_t frameSize)
{
    sampleRate_ = sampleRate;
    frameSize_ = frameSize;
    const auto& inputDevices = YADAW::Native::ALSADeviceEnumerator::audioInputDevices();
    inputs_.reserve(inputDevices.size());
    for(const auto& inputDevice: inputDevices)
    {
        inputs_.emplace_back(
            inputDevice, nullptr, 0U, snd_pcm_format_t {}, snd_pcm_access_t {}, nullptr
        );
    }
    const auto& outputDevices = YADAW::Native::ALSADeviceEnumerator::audioOutputDevices();
    outputs_.reserve(outputDevices.size());
    for(const auto& outputDevice: outputDevices)
    {
        outputs_.emplace_back(
            outputDevice, nullptr, 0U, snd_pcm_format_t {}, snd_pcm_access_t {}, nullptr
        );
    }
}

void ALSABackend::Impl::uninitialize()
{
    stop();
    for(const auto& [selector, pcm, channelCount, format, access, buffer]: inputs_)
    {
        if(pcm)
        {
            snd_pcm_close(pcm);
        }
    }
    for(const auto& [selector, pcm, channelCount, format, access, buffer]: outputs_)
    {
        if(pcm)
        {
            snd_pcm_close(pcm);
        }
    }
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

bool ALSABackend::Impl::start()
{
    if(!runFlag_.test_and_set(std::memory_order::memory_order_acquire))
    {
        std::vector<std::tuple<ALSADeviceSelector, snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t>> inputs;
        std::vector<std::tuple<ALSADeviceSelector, snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t>> outputs;
        audioThread_ = std::thread(
            [this, inputs = std::move(inputs), outputs = std::move(outputs)]()
            {
                int errorCode = 0;
                while(runFlag_.test_and_set(std::memory_order::memory_order_acquire))
                {
                    for(const auto& [selector, pcm, channelCount, format, access]: inputs)
                    {
                        // TODO: audio callback
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

std::tuple<snd_pcm_t*, std::uint32_t, snd_pcm_format_t, snd_pcm_access_t, std::byte*>
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
    std::uint32_t formatIndex = 0;
    for(std::size_t i = 0; i < YADAW::Util::stackArraySize(formats); ++i)
    {
        if(snd_pcm_hw_params_set_format(pcm, hwParams, formats[i]) == 0)
        {
            format = formats[i];
            formatIndex = i;
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
    errorCode = snd_pcm_hw_params_set_buffer_size(pcm, hwParams, frameSize_);
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
    for(std::size_t i = 0; i < YADAW::Util::stackArraySize(accesses); ++i)
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
    errorCode = snd_pcm_sw_params_set_avail_min(pcm, swParams, frameSize_);
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
    errorCode = snd_pcm_prepare(pcm);
    if(errorCode < 0)
    {
        auto errorMessage = snd_strerror(errorCode);
        std::fprintf(stderr, "`snd_pcm_prepare` failed with error code %d: %s\n",
            errorCode, errorMessage);
        snd_pcm_close(pcm);
        return {};
    }
    auto buffer = allocateBuffer(frameSize_, channelCount, formatIndex);
    if(!buffer)
    {
        std::fprintf(stderr, "Allocate buffer failed\n");
        snd_pcm_close(pcm);
        return {};
    }
    auto ptr = buffer.get();
    buffers_.emplace(std::move(buffer));
    return {pcm, channelCount, format, static_cast<snd_pcm_access_t>(access), ptr};
}

std::shared_ptr<std::byte[]> ALSABackend::Impl::allocateBuffer(
    std::uint32_t frameSize, std::uint32_t channelCount, std::uint32_t formatIndex)
{
    assert(formatIndex < YADAW::Util::stackArraySize(formats));
    auto ptr = static_cast<std::byte*>(
        ::operator new[](
            frameSize * channelCount * sampleSize[formatIndex],
            std::align_val_t(sampleSize[formatIndex]), std::nothrow
        )
    );
    if(ptr)
    {
        auto deleter = [align = std::align_val_t(sampleSize[formatIndex])](void* ptr)
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