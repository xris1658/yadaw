#if _WIN32

#include "audio/backend/WASAPIExclusiveBackend.hpp"
#include "native/win/WASAPIExclusiveBackendImpl.hpp"

namespace YADAW::Audio::Backend
{
WASAPIExclusiveBackend::WASAPIExclusiveBackend(
    std::uint32_t sampleRate, std::uint32_t frameCount):
    pImpl_(std::make_unique<Impl>(sampleRate, frameCount))
{}

WASAPIExclusiveBackend::~WASAPIExclusiveBackend()
{
    // TODO
}

std::uint32_t WASAPIExclusiveBackend::inputDeviceCount() const
{
    return pImpl_->inputDeviceCount();
}

std::uint32_t WASAPIExclusiveBackend::outputDeviceCount() const
{
    return pImpl_->outputDeviceCount();
}

std::optional<QString> WASAPIExclusiveBackend::inputDeviceNameAt(
    std::uint32_t index) const
{
    return pImpl_->inputDeviceNameAt(index);
}

std::optional<QString> WASAPIExclusiveBackend::outputDeviceNameAt(
    std::uint32_t index) const
{
    return pImpl_->outputDeviceNameAt(index);
}

std::optional<QString> WASAPIExclusiveBackend::inputDeviceIdAt(
    std::uint32_t index) const
{
    return pImpl_->inputDeviceIdAt(index);
}

std::optional<QString> WASAPIExclusiveBackend::outputDeviceIdAt(
    std::uint32_t index) const
{
    return pImpl_->outputDeviceIdAt(index);
}

QString WASAPIExclusiveBackend::defaultInputDeviceId() const
{
    return pImpl_->defaultInputDeviceID();
}

QString WASAPIExclusiveBackend::defaultOutputDeviceId() const
{
    return pImpl_->defaultOutputDeviceID();
}

std::optional<bool> WASAPIExclusiveBackend::isInputDeviceActivated(
    std::uint32_t index) const
{
    return pImpl_->isInputDeviceActivated(index);
}

std::optional<bool> WASAPIExclusiveBackend::isOutputDeviceActivated(
    std::uint32_t index) const
{
    return pImpl_->isOutputDeviceActivated(index);
}

YADAW::Native::ErrorCodeType WASAPIExclusiveBackend::activateInputDevice(
    std::uint32_t index, bool activate)
{
    return pImpl_->activateInputDevice(index, activate);
}

YADAW::Native::ErrorCodeType WASAPIExclusiveBackend::activateOutputDevice(
    std::uint32_t index, bool activate)
{
    return pImpl_->activateOutputDevice(index, activate);
}
}

#endif