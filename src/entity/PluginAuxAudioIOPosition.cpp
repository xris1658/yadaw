#include "PluginAuxAudioIOPosition.hpp"

#include "model/MixerAudioIOPositionItemModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
PluginAuxAudioIOPosition::PluginAuxAudioIOPosition(
    const QModelIndex& index, QObject* parent):
    IAudioIOPosition(parent),
    index_(index)
{
    YADAW::Util::setCppOwnership(*this);
}

PluginAuxAudioIOPosition::~PluginAuxAudioIOPosition()
{}

IAudioIOPosition::Type PluginAuxAudioIOPosition::getType() const
{
    return IAudioIOPosition::Type::PluginAuxIO;
}

QString PluginAuxAudioIOPosition::getName() const
{
    return getModel().data(
        index_,
        YADAW::Model::MixerAudioIOPositionItemModel::Role::TreeName
    ).value<QString>();
}

QString PluginAuxAudioIOPosition::getCompleteName() const
{
    return getName(); // TODO
}

const YADAW::Model::MixerAudioIOPositionItemModel& PluginAuxAudioIOPosition::getModel() const
{
    return *static_cast<const YADAW::Model::MixerAudioIOPositionItemModel*>(index_.model());
}

PluginAuxAudioIOPosition::operator YADAW::Audio::Mixer::Mixer::Position() const
{
    return getModel().getPosition(index_).value_or(
        YADAW::Audio::Mixer::Mixer::Position {
            .type = YADAW::Audio::Mixer::Mixer::Position::Type::Invalid,
            .id = YADAW::Audio::Mixer::IDGen::InvalidId
        }
    );
}
}
