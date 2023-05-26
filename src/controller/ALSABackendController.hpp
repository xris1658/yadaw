#ifndef YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER

#include "audio/backend/ALSABackend.hpp"
#include "model/ALSAInputDeviceListModel.hpp"
#include "model/ALSAOutputDeviceListModel.hpp"

namespace YADAW::Controller
{
constexpr double defaultSampleRate = 44100.0;

YADAW::Audio::Backend::ALSABackend& appALSABackend();

YADAW::Model::ALSAInputDeviceListModel& appALSAInputDeviceListModel();

YADAW::Model::ALSAOutputDeviceListModel& appALSAOutputDeviceListModel();

void activateDefaultDevice(YADAW::Audio::Backend::ALSABackend& backend);

bool initializeALSAFromConfig(const YAML::Node& node);

YAML::Node deviceConfigFromALSA();
}

#endif //YADAW_SRC_CONTROLLER_ALSABACKENDCONTROLLER
