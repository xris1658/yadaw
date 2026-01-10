#ifndef YADAW_SRC_ENTITY_HARDWAREAUDIOIOPOSITION
#define YADAW_SRC_ENTITY_HARDWAREAUDIOIOPOSITION

#include "audio/mixer/Mixer.hpp"
#include "entity/IAudioIOPosition.hpp"

namespace YADAW::Model
{
class MixerChannelListModel;
}

namespace YADAW::Entity
{
class HardwareAudioIOPosition: public IAudioIOPosition
{
    Q_OBJECT
    friend class YADAW::Model::MixerChannelListModel;
private:
    HardwareAudioIOPosition(
        YADAW::Model::MixerChannelListModel& model,
        std::uint32_t index,
        QObject* parent = nullptr
    );
public:
    ~HardwareAudioIOPosition() override;
public:
    Type getType() const override;
    QString getName() const override;
    QString getCompleteName() const override;
public:
    const YADAW::Model::MixerChannelListModel& getModel() const;
    std::uint32_t getIndex() const;
private:
    void updateIndex(std::uint32_t index);
public:
    operator YADAW::Audio::Mixer::Mixer::Position() const;
private:
    YADAW::Model::MixerChannelListModel* model_;
    std::uint32_t index_;
};
}

#endif // YADAW_SRC_ENTITY_HARDWAREAUDIOIOPOSITION
