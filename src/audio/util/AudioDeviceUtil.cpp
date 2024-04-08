#include "AudioDeviceUtil.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Audio::Util
{
std::uint32_t firstMainAudioInput(const YADAW::Audio::Device::IAudioDevice& device)
{
    auto count = device.audioInputGroupCount();
    FOR_RANGE0(i, count)
    {
        if(device.audioInputGroupAt(i)->get().isMain())
        {
            return i;
        }
    }
    return count;
}

std::uint32_t firstMainAudioOutput(const YADAW::Audio::Device::IAudioDevice& device)
{
    auto count = device.audioOutputGroupCount();
    FOR_RANGE0(i, count)
    {
        if(device.audioOutputGroupAt(i)->get().isMain())
        {
            return i;
        }
    }
    return count;
}
}