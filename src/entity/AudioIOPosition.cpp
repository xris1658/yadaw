#include "entity/AudioIOPosition.hpp"

namespace YADAW::Entity
{
YADAW::Audio::Mixer::Mixer::Position::Type fromEntityType(
    AudioIOPosition::Type entityType)
{
    switch(entityType)
    {
    case AudioIOPosition::Type::AudioHardwareIOChannel:
        return YADAW::Audio::Mixer::Mixer::Position::Type::AudioHardwareIOChannel;
    case AudioIOPosition::Type::BusAndFXChannel:
        return YADAW::Audio::Mixer::Mixer::Position::Type::BusAndFXChannel;
    case AudioIOPosition::Type::SidechainOfPlugin:
        return YADAW::Audio::Mixer::Mixer::Position::Type::SidechainOfPlugin;
    default:
        return YADAW::Audio::Mixer::Mixer::Position::Type::Invalid;
    }
}

AudioIOPosition::Type toEntityType(
    YADAW::Audio::Mixer::Mixer::Position::Type entityType)
{
    switch(entityType)
    {
    case YADAW::Audio::Mixer::Mixer::Position::Type::AudioHardwareIOChannel:
        return AudioIOPosition::Type::AudioHardwareIOChannel;
    case YADAW::Audio::Mixer::Mixer::Position::Type::BusAndFXChannel:
        return AudioIOPosition::Type::BusAndFXChannel;
    case YADAW::Audio::Mixer::Mixer::Position::Type::SidechainOfPlugin:
        return AudioIOPosition::Type::SidechainOfPlugin;
    default:
        return AudioIOPosition::Type::Invalid;
    }
}

AudioIOPosition::AudioIOPosition(QObject* parent):
    QObject(parent),
    position_()
{}

AudioIOPosition::AudioIOPosition(
    const YADAW::Audio::Mixer::Mixer::Position& position):
    QObject(nullptr),
    position_(position)
{}

AudioIOPosition::AudioIOPosition(const AudioIOPosition& rhs):
    QObject(rhs.parent()),
    position_(rhs.position_)
{}

AudioIOPosition::~AudioIOPosition()
{}

AudioIOPosition::Type AudioIOPosition::getType() const
{
    return toEntityType(position_.type);
}

void AudioIOPosition::setType(Type type)
{
    position_.type = fromEntityType(type);
    typeChanged();
}

int AudioIOPosition::getChannelGroupIndex() const
{
    return position_.channelGroupIndex;
}

void AudioIOPosition::setChannelGroupIndex(int channelGroupIndex)
{
    position_.channelGroupIndex = channelGroupIndex;
    channelGroupIndexChanged();
}

QString AudioIOPosition::getId() const
{
    return QString::number(position_.id);
}

void AudioIOPosition::setId(const QString& id)
{
    auto convertOk = false;
    auto idAsNumber = id.toULongLong(&convertOk);
    if(convertOk)
    {
        position_.id = idAsNumber;
        idChanged();
    }
}
}
