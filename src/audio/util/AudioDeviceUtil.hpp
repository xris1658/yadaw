#ifndef YADAW_SRC_AUDIO_UTIL_AUDIODEVICEUTIL
#define YADAW_SRC_AUDIO_UTIL_AUDIODEVICEUTIL

#include "audio/device/IAudioDevice.hpp"

namespace YADAW::Audio::Util
{
std::uint32_t firstMainAudioInput(const YADAW::Audio::Device::IAudioDevice& device);

std::uint32_t firstMainAudioOutput(const YADAW::Audio::Device::IAudioDevice& device);
}

#endif //YADAW_SRC_AUDIO_UTIL_AUDIODEVICEUTIL
