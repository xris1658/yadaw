#include "ALSABackendController.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Backend::ALSAAudioBackend& appALSABackend()
{
    static YADAW::Audio::Backend::ALSAAudioBackend ret;
    return ret;
}

YADAW::Model::ALSAInputDeviceListModel& appALSAInputDeviceListModel()
{
    static YADAW::Model::ALSAInputDeviceListModel ret(appALSABackend());
    return ret;
}

YADAW::Model::ALSAOutputDeviceListModel& appALSAOutputDeviceListModel()
{
    static YADAW::Model::ALSAOutputDeviceListModel ret(appALSABackend());
    return ret;
}
}