#ifndef YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER

#include "audio/backend/ALSABackend.hpp"
#include "audio/backend/ALSABusConfiguration.hpp"
#include "model/ALSAInputDeviceListModel.hpp"
#include "model/ALSAOutputDeviceListModel.hpp"

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
constexpr std::uint32_t DefaultSampleRate = 44100U;

constexpr std::uint32_t DefaultFrameSize = 512U;

YADAW::Audio::Backend::ALSABackend& appALSABackend();

YADAW::Model::ALSAInputDeviceListModel& appALSAInputDeviceListModel();

YADAW::Model::ALSAOutputDeviceListModel& appALSAOutputDeviceListModel();

void activateDefaultDevice(YADAW::Audio::Backend::ALSABackend& backend);

bool initializeALSAFromConfig(const YAML::Node& node);

YAML::Node deviceConfigFromALSA();

YADAW::Audio::Backend::ALSABusConfiguration& appAudioBusConfiguration();
}

#endif // YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER
