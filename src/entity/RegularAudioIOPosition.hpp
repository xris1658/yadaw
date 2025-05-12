#ifndef YADAW_SRC_ENTITY_REGULARAUDIOIOPOSTION
#define YADAW_SRC_ENTITY_REGULARAUDIOIOPOSTION

#include "audio/mixer/Mixer.hpp"
#include "entity/IAudioIOPosition.hpp"

namespace YADAW::Model
{
class RegularAudioIOPositionModel;
}

namespace YADAW::Entity
{
class RegularAudioIOPosition: public IAudioIOPosition
{
    Q_OBJECT
    friend class YADAW::Model::RegularAudioIOPositionModel;
private:
    RegularAudioIOPosition(
        YADAW::Model::RegularAudioIOPositionModel& model,
        std::uint32_t index,
        QObject* parent = nullptr
    );
public:
    ~RegularAudioIOPosition() override;
public:
    Type getType() const override;
    QString getName() const override;
    QString getCompleteName() const override;
public:
    const YADAW::Model::RegularAudioIOPositionModel& getModel() const;
    std::uint32_t getIndex() const;
private:
    void updateIndex(std::uint32_t index);
public:
    operator YADAW::Audio::Mixer::Mixer::Position() const;
private:
    YADAW::Model::RegularAudioIOPositionModel* model_;
    std::uint32_t index_;
};
}

#endif // YADAW_SRC_ENTITY_REGULARAUDIOIOPOSTION
