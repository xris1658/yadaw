#ifndef YADAW_SRC_CONTROLLER_AUDIOBUSCONFIGURATIONCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOBUSCONFIGURATIONCONTROLLER

#include "model/AudioBusConfigurationModel.hpp"

#include <yaml-cpp/yaml.h>

namespace YADAW::Controller
{
void loadAudioBusConfiguration(
    const YAML::Node& node, 
    YADAW::Model::AudioBusConfigurationModel& inputModel,
    YADAW::Model::AudioBusConfigurationModel& outputModel);

YAML::Node exportFromAudioBusConfiguration(
    const YADAW::Audio::Device::IAudioBusConfiguration& configuration,
    const YADAW::Model::AudioBusConfigurationModel& inputModel,
    const YADAW::Model::AudioBusConfigurationModel& outputModel);
}

#endif //YADAW_SRC_CONTROLLER_AUDIOBUSCONFIGURATIONCONTROLLER
