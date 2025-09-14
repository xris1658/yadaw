#ifndef YADAW_SRC_MODEL_AUXIOTARGETLISTMODEL
#define YADAW_SRC_MODEL_AUXIOTARGETLISTMODEL

#include "model/IAuxIOTargetListModel.hpp"

#include "audio/mixer/Mixer.hpp"

namespace YADAW::Model
{
class AuxIOTargetListModel: public IAuxIOTargetListModel
{
public:
    AuxIOTargetListModel(
        YADAW::Audio::Mixer::Mixer& mixer,
        YADAW::Audio::Mixer::Mixer::ChannelListType channelListType,
        std::uint32_t channelIndex,
        bool isInstrument, bool isPreFaderInsert,
        std::uint32_t insertIndex,
        QObject* parent = nullptr);
    virtual ~AuxIOTargetListModel() override;
public:
    YADAW::Audio::Mixer::Mixer::PluginAuxIOPosition position(std::uint32_t channelGroupIndex) const;
protected:
    YADAW::Audio::Mixer::Mixer* mixer_;
    std::uint8_t channelListType_: 2;
    std::uint8_t isInstrument_: 1;
    std::uint8_t isPreFaderInsert_: 1;
    std::uint32_t channelIndex_;
    std::uint32_t insertIndex_;
};
}

#endif // YADAW_SRC_MODEL_AUXIOTARGETLISTMODEL