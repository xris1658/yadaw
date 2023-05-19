#if(__linux__)

#include "ALSAAudioBackendImpl.hpp"

#include <filesystem>
#include <mutex>

std::once_flag flag;

namespace YADAW::Audio::Backend
{
std::uint32_t ALSAAudioBackend::Impl::audioInputCount()
{
    return YADAW::Native::ALSADeviceEnumerator::audioInputDeviceCount();
}

std::uint32_t ALSAAudioBackend::Impl::audioOutputCount()
{
    return YADAW::Native::ALSADeviceEnumerator::audioOutputDeviceCount();
}

std::optional<ALSAAudioBackend::Impl::PCMDeviceSelector>
    ALSAAudioBackend::Impl::audioInputDeviceAt(std::uint32_t index)
{
    return YADAW::Native::ALSADeviceEnumerator::audioInputDeviceAt(index);
}

std::optional<ALSAAudioBackend::Impl::PCMDeviceSelector>
    ALSAAudioBackend::Impl::audioOutputDeviceAt(std::uint32_t index)
{
    return YADAW::Native::ALSADeviceEnumerator::audioOutputDeviceAt(index);
}
}

#endif