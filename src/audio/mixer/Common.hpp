#ifndef YADAW_SRC_AUDIO_MIXER_COMMON
#define YADAW_SRC_AUDIO_MIXER_COMMON

#include "audio/device/IAudioDevice.hpp"
#include "audio/mixer/PolarityInverter.hpp"
#include "audio/mixer/Meter.hpp"
#include "audio/mixer/VolumeFader.hpp"
#include "audio/util/Summing.hpp"
#include "audio/util/Mute.hpp"
#include "util/AutoIncrementID.hpp"
#include "util/PolymorphicDeleter.hpp"

#include <ade/node.hpp>
#include <ade/handle.hpp>

namespace YADAW::Audio::Mixer
{
template<typename T = YADAW::Audio::Device::IAudioDevice>
    requires std::derived_from<T, YADAW::Audio::Device::IAudioDevice>
    || std::is_same_v<YADAW::Audio::Device::IAudioDevice, T>
using DeviceAndNode = std::pair<std::unique_ptr<T>, ade::NodeHandle>;
using PolarityInverterAndNode = DeviceAndNode<YADAW::Audio::Mixer::PolarityInverter>;
using MeterAndNode            = DeviceAndNode<YADAW::Audio::Mixer::Meter>;
using FaderAndNode            = DeviceAndNode<YADAW::Audio::Mixer::VolumeFader>;
using SummingAndNode          = DeviceAndNode<YADAW::Audio::Util::Summing>;
using MuteAndNode             = DeviceAndNode<YADAW::Audio::Util::Mute>;

using IDGen = YADAW::Util::AutoIncrementID;
using Context = YADAW::Util::PMRUniquePtr<void>;
}

#endif // YADAW_SRC_AUDIO_MIXER_COMMON