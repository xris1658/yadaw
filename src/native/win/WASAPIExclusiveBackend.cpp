#if _WIN32

#include "audio/backend/WASAPIExclusiveBackend.hpp"
#include "native/win/WASAPIExclusiveBackendImpl.hpp"

namespace YADAW::Audio::Backend
{
WASAPIExclusiveBackend::WASAPIExclusiveBackend():
    pImpl_()
{}

WASAPIExclusiveBackend::~WASAPIExclusiveBackend()
{
    // TODO
}

std::uint32_t WASAPIExclusiveBackend::audioInputDeviceCount() const
{
    return pImpl_->inputDeviceCount();
}

std::uint32_t WASAPIExclusiveBackend::audioOutputDeviceCount() const
{
    return pImpl_->outputDeviceCount();
}

std::optional<QString> WASAPIExclusiveBackend::audioInputDeviceNameAt(
    std::uint32_t index) const
{
    return pImpl_->inputDeviceNameAt(index);
}

std::optional<QString> WASAPIExclusiveBackend::audioOutputDeviceNameAt(
    std::uint32_t index) const
{
    return pImpl_->outputDeviceNameAt(index);
}

std::optional<QString> WASAPIExclusiveBackend::audioInputDeviceIdAt(
    std::uint32_t index) const
{
    return pImpl_->inputDeviceIdAt(index);
}

std::optional<QString> WASAPIExclusiveBackend::audioOutputDeviceIdAt(
    std::uint32_t index) const
{
    return pImpl_->outputDeviceIdAt(index);
}
}

#endif