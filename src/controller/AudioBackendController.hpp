#ifndef YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
#define YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER

#include "audio/backend/AudioGraphBackend.hpp"
#include "model/AudioGraphInputDeviceListModel.hpp"
#include "model/AudioGraphOutputDeviceListModel.hpp"

namespace YADAW::Controller
{
enum AudioBackend
{
    Dummy,
    AudioGraph
};

YADAW::Audio::Backend::AudioGraphBackend& appAudioGraphBackend();

YADAW::Model::AudioGraphInputDeviceListModel& appAudioGraphInputDeviceListModel();

YADAW::Model::AudioGraphOutputDeviceListModel& appAudioGraphOutputDeviceListModel();
}

#endif //YADAW_SRC_CONTROLLER_AUDIOBACKENDCONTROLLER
