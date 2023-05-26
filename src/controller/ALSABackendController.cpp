#include "ALSABackendController.hpp"

namespace YADAW::Controller
{
YADAW::Audio::Backend::ALSABackend& appALSABackend()
{
    static YADAW::Audio::Backend::ALSABackend ret;
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