#ifndef YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER

#if __linux__

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

std::vector<QString> activateDefaultDevice(
    YADAW::Audio::Backend::ALSABackend& backend);

std::vector<QString> initializeALSAFromConfig(const YAML::Node& node);

YAML::Node deviceConfigFromALSA();

YADAW::Audio::Backend::ALSABusConfiguration& appAudioBusConfiguration();

void alsaCallback(const YADAW::Audio::Backend::ALSABackend* backend,
        std::uint32_t inputCount, YADAW::Audio::Backend::ALSABackend::AudioBuffer* const inputBuffers,
        std::uint32_t outputCount, YADAW::Audio::Backend::ALSABackend::AudioBuffer* const outputBuffers);
}

#endif

#endif // YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER
