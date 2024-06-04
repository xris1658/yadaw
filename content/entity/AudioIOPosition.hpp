#ifndef YADAW_CONTENT_ENTITY_AUDIOIOPOSITION
#define YADAW_CONTENT_ENTITY_AUDIOIOPOSITION

#include "audio/mixer/Mixer.hpp"

#include <QObject>
#include <QString>

namespace YADAW::Entity
{
class AudioIOPosition: public QObject
{
    Q_OBJECT
    Q_PROPERTY(Type type READ getType WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int channelGroupIndex READ getChannelGroupIndex WRITE setChannelGroupIndex NOTIFY channelGroupIndexChanged)
    Q_PROPERTY(QString id READ getId WRITE setId NOTIFY idChanged)
public:
    enum Type
    {
        Invalid,
        AudioHardwareIOChannel,
        BusAndFXChannel,
        SidechainOfPlugin
    };
    Q_ENUM(Type);
public:
    AudioIOPosition(QObject* parent = nullptr);
    AudioIOPosition(const YADAW::Audio::Mixer::Mixer::Position& position);
    AudioIOPosition(const AudioIOPosition& rhs);
    ~AudioIOPosition() override;
public:
    Type getType() const;
    void setType(Type type);
    int getChannelGroupIndex() const;
    void setChannelGroupIndex(int channelGroupIndex);
    QString getId() const;
    void setId(const QString& id);
    signals:
    void typeChanged();
    void channelGroupIndexChanged();
    void idChanged();
public:
    YADAW::Audio::Mixer::Mixer::Position position_;
};
}

Q_DECLARE_METATYPE(YADAW::Entity::AudioIOPosition)

#endif // YADAW_CONTENT_ENTITY_AUDIOIOPOSITION
