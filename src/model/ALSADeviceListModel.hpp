#ifndef YADAW_SRC_MODEL_ALSADEVICELISTMODEL
#define YADAW_SRC_MODEL_ALSADEVICELISTMODEL

#include "audio/backend/ALSAAudioBackend.hpp"
#include "model/IAudioDeviceListModel.hpp"

namespace YADAW::Model
{
class ALSADeviceListModel: public YADAW::Model::IAudioDeviceListModel
{
private:
    YADAW::Audio::Backend::ALSAAudioBackend* backend_;
};
}

#endif //YADAW_SRC_MODEL_ALSADEVICELISTMODEL
