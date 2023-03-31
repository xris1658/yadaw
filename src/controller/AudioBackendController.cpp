#include "AudioBackendController.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Backend::AudioGraphBackend& appAudioGraphBackend()
{
    static YADAW::Audio::Backend::AudioGraphBackend ret;
    return ret;
}

YADAW::Model::AudioGraphInputDeviceListModel& appAudioGraphInputDeviceListModel()
{
    static YADAW::Model::AudioGraphInputDeviceListModel ret(appAudioGraphBackend());
    return ret;
}

YADAW::Model::AudioGraphOutputDeviceListModel& appAudioGraphOutputDeviceListModel()
{
    static YADAW::Model::AudioGraphOutputDeviceListModel ret(appAudioGraphBackend());
    return ret;
}
}