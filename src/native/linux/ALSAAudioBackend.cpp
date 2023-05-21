#include "audio/backend/ALSAAudioBackend.hpp"
#include "native/linux/ALSAAudioBackendImpl.hpp"

namespace YADAW::Audio::Backend
{
std::uint32_t ALSAAudioBackend::audioInputDeviceCount()
{
    return Impl::audioInputCount();
}

std::uint32_t ALSAAudioBackend::audioOutputDeviceCount()
{
    return Impl::audioOutputCount();
}

std::optional<ALSADeviceSelector> ALSAAudioBackend::audioInputDeviceAt(std::uint32_t index)
{
    return Impl::audioInputDeviceAt(index);
}

std::optional<ALSADeviceSelector> ALSAAudioBackend::audioOutputDeviceAt(std::uint32_t index)
{
    return Impl::audioOutputDeviceAt(index);
}
}