#if __APPLE__

#include "CoreAudioBackendImpl.hpp"

#include "util/IntegerRange.hpp"

#include <../../System/Library/Frameworks/CoreFoundation.framework/Headers/CFString.h>
#include <../../System/Library/Frameworks/CoreAudioTypes.Framework/Headers/CoreAudioTypes.h>

#include <mutex>
#include <vector>

namespace YADAW::Audio::Backend
{
std::once_flag enumerateAudioDevicesFlag;

std::vector<CoreAudioBackend::DeviceInfo> inputDevices;

std::vector<CoreAudioBackend::DeviceInfo> outputDevices;

void doEnumerateAudioDevices(bool isInput, std::vector<CoreAudioBackend::DeviceInfo>& vec)
{
    AudioObjectPropertyScope scope[2] = {kAudioObjectPropertyScopeOutput, kAudioObjectPropertyScopeInput};
    AudioObjectPropertyAddress address {
        .mSelector = kAudioHardwarePropertyDevices,
        .mScope = scope[isInput],
        .mElement = kAudioObjectPropertyElementMain
    };
    std::uint32_t propertySize = 0;
    AudioObjectGetPropertyDataSize(
        static_cast<AudioObjectID>(kAudioObjectSystemObject),
        &address,
        0, nullptr, &propertySize
    );
    auto count = propertySize / sizeof(std::uint32_t);
    std::vector<AudioDeviceID> ids(count);
    vec.reserve(count);
    AudioObjectGetPropertyData(
        static_cast<AudioObjectID>(kAudioObjectSystemObject),
        &address,
        0, nullptr, &propertySize,
        ids.data()
    );
    FOR_RANGE0(i, count)
    {
        address.mSelector = kAudioDevicePropertyStreamConfiguration;
        auto channelCount = 0;
        {
            AudioObjectGetPropertyDataSize(
                ids[i], &address, 0, nullptr, &propertySize
            );
            auto audioBufferList = std::make_unique<AudioBufferList>();
            AudioObjectGetPropertyData(
                ids[i],
                &address,
                0, nullptr, &propertySize, audioBufferList.get()
            );
            FOR_RANGE0(i, audioBufferList->mNumberBuffers)
            {
                channelCount += audioBufferList->mBuffers[i].mNumberChannels;
            }
        }
        if(channelCount)
        {
            address.mSelector = kAudioDevicePropertyDeviceNameCFString;
            CFStringRef name = nullptr;
            propertySize = sizeof(name);
            AudioObjectGetPropertyData(
                ids[i],
                &address,
                0, nullptr, &propertySize, &name
            );
            vec.emplace_back(
                ids[i], QString::fromCFString(name)
            );
            CFRelease(name);
        }
    }
}

void enumerateAudioDevices()
{
    doEnumerateAudioDevices(true, inputDevices);
    doEnumerateAudioDevices(false, outputDevices);
}

AudioDeviceID defaultDevice(bool isInput)
{
    AudioObjectPropertySelector selector[2] = {kAudioHardwarePropertyDefaultOutputDevice, kAudioHardwarePropertyDefaultInputDevice};
    AudioObjectPropertyScope scope[2] = {kAudioObjectPropertyScopeOutput, kAudioObjectPropertyScopeInput};
    AudioObjectPropertyAddress address {
        .mSelector = selector[isInput],
        .mScope = scope[isInput],
        .mElement = kAudioObjectPropertyElementMain
    };
    AudioDeviceID ret;
    std::uint32_t propertySize = sizeof(AudioDeviceID);
    AudioObjectGetPropertyData(
        static_cast<AudioObjectID>(kAudioObjectSystemObject),
        &address,
        0, nullptr, &propertySize, &ret
    );
    return ret;
}

std::uint32_t CoreAudioBackend::Impl::audioInputDeviceCount()
{
    std::call_once(enumerateAudioDevicesFlag, &enumerateAudioDevices);
    return inputDevices.size();
}

std::uint32_t CoreAudioBackend::Impl::audioOutputDeviceCount()
{
    std::call_once(enumerateAudioDevicesFlag, &enumerateAudioDevices);
    return outputDevices.size();
}

std::optional<CoreAudioBackend::DeviceInfo> CoreAudioBackend::Impl::audioInputDeviceAt(std::uint32_t index)
{
    std::call_once(enumerateAudioDevicesFlag, &enumerateAudioDevices);
    if(index < inputDevices.size())
    {
        return inputDevices[index];
    }
    return std::nullopt;
}

std::optional<CoreAudioBackend::DeviceInfo> CoreAudioBackend::Impl::audioOutputDeviceAt(std::uint32_t index)
{
    std::call_once(enumerateAudioDevicesFlag, &enumerateAudioDevices);
    if(index < outputDevices.size())
    {
        return outputDevices[index];
    }
    return std::nullopt;
}

AudioDeviceID CoreAudioBackend::Impl::defaultInputDevice()
{
    return defaultDevice(true);
}

AudioDeviceID CoreAudioBackend::Impl::defaultOutputDevice()
{
    return defaultDevice(false);
}

std::optional<double> CoreAudioBackend::Impl::deviceNominalSampleRate(
    bool isInput, AudioDeviceID deviceID)
{
    AudioObjectPropertyScope scope[2] = {
        kAudioObjectPropertyScopeOutput,
        kAudioObjectPropertyScopeInput
    };
    double ret;
    std::uint32_t size = sizeof(ret);
    AudioObjectPropertyAddress address {
        .mSelector = kAudioDevicePropertyNominalSampleRate,
        .mScope = scope[isInput],
        .mElement = kAudioObjectPropertyElementMain
    };
    auto status = AudioObjectGetPropertyData(
        deviceID,
        &address,
        0,
        nullptr,
        &size,
        &ret
    );
    if (status == 0)
    {
        return {ret};
    }
    return std::nullopt;
}

std::optional<std::vector<CoreAudioBackend::SampleRateRange>>
CoreAudioBackend::Impl::deviceAvailableNominalSampleRates(
    bool isInput, AudioDeviceID deviceID)
{
    AudioObjectPropertyScope scope[2] = {
        kAudioObjectPropertyScopeOutput,
        kAudioObjectPropertyScopeInput
    };
    AudioObjectPropertyAddress address {
        .mSelector = kAudioDevicePropertyAvailableNominalSampleRates,
        .mScope = scope[isInput],
        .mElement = kAudioObjectPropertyElementMain
    };
    std::uint32_t size = 0;
    auto status = AudioObjectGetPropertyDataSize(
        deviceID,
        &address,
        0, nullptr,
        &size
    );
    if(status == 0)
    {
        std::vector<AudioValueRange> ranges;
        std::uint32_t size2 = size;
        ranges.resize(size2 / sizeof(AudioValueRange));
        status = AudioObjectGetPropertyData(
            deviceID,
            &address,
            0, nullptr,
            &size2, ranges.data()
        );
        std::vector<SampleRateRange> ret;
        ret.reserve(ranges.size());
        for(std::uint32_t i = 0; i < ranges.size(); ++i)
        {
            ret.emplace_back(
                ranges[i].mMinimum,
                ranges[i].mMaximum
            );
        }
        return {ret};
    }
    return std::nullopt;
}
}

#endif