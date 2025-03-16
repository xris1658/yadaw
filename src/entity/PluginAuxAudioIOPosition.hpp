#ifndef YADAW_SRC_ENTITY_PLUGINAUXAUDIOIOPOSITION
#define YADAW_SRC_ENTITY_PLUGINAUXAUDIOIOPOSITION

#include "audio/mixer/Mixer.hpp"
#include "entity/IAudioIOPosition.hpp"
#include "model/HardwareAudioIOPositionModel.hpp"

namespace YADAW::Model
{
class MixerAudioIOPositionItemModel;
}

namespace YADAW::Entity
{
class PluginAuxAudioIOPosition: public IAudioIOPosition
{
    Q_OBJECT
    friend class YADAW::Model::MixerAudioIOPositionItemModel;
private:
    PluginAuxAudioIOPosition(
        YADAW::Model::MixerAudioIOPositionItemModel& model,
        const QModelIndex& index,
        QObject* parent = nullptr
    );
public:
    ~PluginAuxAudioIOPosition() override;
public:
    Type getType() const override;
    QString getName() const override;
    QString getCompleteName() const override;
public:
    const YADAW::Model::MixerAudioIOPositionItemModel& getModel() const;
public:
    operator YADAW::Audio::Mixer::Mixer::Position() const;
private:
    YADAW::Model::MixerAudioIOPositionItemModel* model_;
    QModelIndex index_;
};
}

#endif // YADAW_SRC_ENTITY_PLUGINAUXAUDIOIOPOSITION