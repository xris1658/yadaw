#ifndef YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER

#include "audio/backend/AudioGraphBackend.hpp"
#include "model/AudioGraphInputDeviceListModel.hpp"
#include "model/AudioGraphOutputDeviceListModel.hpp"

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
constexpr double defaultSampleRate = 44100.0;

YADAW::Audio::Backend::AudioGraphBackend& appAudioGraphBackend();

YADAW::Model::AudioGraphInputDeviceListModel& appAudioGraphInputDeviceListModel();

YADAW::Model::AudioGraphOutputDeviceListModel& appAudioGraphOutputDeviceListModel();

void activateDefaultDevice(YADAW::Audio::Backend::AudioGraphBackend& backend);

bool createAudioGraphFromConfig(const YAML::Node& node);

YAML::Node deviceConfigFromCurrentAudioGraph();
}

#endif //YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
