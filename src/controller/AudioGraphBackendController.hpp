#ifndef YADAW_SRC_CONTROLLER_AUDIOGRAPHBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOGRAPHBACKENDCONTROLLER

#if _WIN32

#include "audio/backend/AudioGraphBackend.hpp"
#include "audio/backend/AudioGraphBusConfiguration.hpp"
#include "model/AudioGraphInputDeviceListModel.hpp"
#include "model/AudioGraphOutputDeviceListModel.hpp"
#include "model/AudioBusConfigurationModel.hpp"

#include <yaml-cpp/yaml.h>

#include <cstdint>
#include <vector>

namespace YADAW::Controller
{
constexpr double defaultSampleRate = 44100.0;

YADAW::Audio::Backend::AudioGraphBackend& appAudioGraphBackend();

YADAW::Model::AudioGraphInputDeviceListModel& appAudioGraphInputDeviceListModel();

YADAW::Model::AudioGraphOutputDeviceListModel& appAudioGraphOutputDeviceListModel();

YADAW::Native::ErrorCodeType activateDefaultDevice(YADAW::Audio::Backend::AudioGraphBackend& backend);

YADAW::Native::ErrorCodeType createAudioGraphFromConfig(const YAML::Node& node);

// Since audio input device list might change between sessions, we have to get
// the change list before initializing audio input buses.
std::vector<std::optional<std::uint32_t>> getAudioInputDeviceIndexChanges(
    const YAML::Node& node
);

YAML::Node deviceConfigFromCurrentAudioGraph();

YADAW::Audio::Backend::AudioGraphBusConfiguration& appAudioBusConfiguration();

void audioGraphCallback(int inputCount, const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* inputs,
    int outputCount, const YADAW::Audio::Backend::AudioGraphBackend::InterleaveAudioBuffer* outputs);
}

#endif

#endif // YADAW_SRC_CONTROLLER_AUDIOGRAPHBACKENDCONTROLLER
