#include "PluginAuxAudioIOPosition.hpp"

#include "model/MixerAudioIOPositionItemModel.hpp"
#include "util/QmlUtil.hpp"

namespace YADAW::Entity
{
PluginAuxAudioIOPosition::PluginAuxAudioIOPosition(
    YADAW::Model::MixerAudioIOPositionItemModel& model,
    const QModelIndex& index, QObject* parent):
    IAudioIOPosition(parent),
    model_(&model),
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
    return model_->data(
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
    return *model_;
}

PluginAuxAudioIOPosition::operator YADAW::Audio::Mixer::Mixer::Position() const
{
    return model_->getPosition(index_).value_or(
        YADAW::Audio::Mixer::Mixer::Position {
            .type = YADAW::Audio::Mixer::Mixer::Position::Type::Invalid,
            .id = YADAW::Audio::Mixer::IDGen::InvalidId
        }
    );
}
}
