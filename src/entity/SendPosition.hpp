#ifndef YADAW_SRC_ENTITY_SENDPOSITION
#define YADAW_SRC_ENTITY_SENDPOSITION

#include "audio/mixer/Mixer.hpp"
#include "entity/IAudioIOPosition.hpp"

namespace YADAW::Model
{
class MixerChannelSendListModel;
}

namespace YADAW::Entity
{
class SendPosition: public IAudioIOPosition
{
    Q_OBJECT
    friend class YADAW::Model::MixerChannelSendListModel;
public:
    SendPosition(YADAW::Model::MixerChannelSendListModel& model,
        std::uint32_t index);
    ~SendPosition() override;
public:
    Type getType() const override;
    QString getName() const override;
    QString getCompleteName() const override;
public:
    const YADAW::Model::MixerChannelSendListModel& getModel() const;
          YADAW::Model::MixerChannelSendListModel& getModel();
    void updateSendIndex(std::uint32_t index);
public:
    operator YADAW::Audio::Mixer::Mixer::Position() const;
private:
    YADAW::Model::MixerChannelSendListModel* model_;
    std::uint32_t index_;
};
}

#endif // YADAW_SRC_ENTITY_SENDPOSITION