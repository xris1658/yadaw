#include "AudioDeviceAuxIOGroupListModel.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
AudioDeviceAuxIOGroupListModel::AudioDeviceAuxIOGroupListModel(
    const IAudioDeviceIOGroupListModel& sourceModel,
    std::uint32_t excludeIndex, QObject* parent):
    IAudioDeviceIOGroupListModel(parent),
    sourceModel_(&sourceModel), excludeIndex_(excludeIndex)
{
    YADAW::Util::setCppOwnership(*this);
}

AudioDeviceAuxIOGroupListModel::~AudioDeviceAuxIOGroupListModel()
{}

int AudioDeviceAuxIOGroupListModel::itemCount() const
{
    return sourceModel_->rowCount() - 1;
}

int AudioDeviceAuxIOGroupListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant AudioDeviceAuxIOGroupListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    return sourceModel_->data(sourceModel_->index(row + (row >= excludeIndex_)), role);
}
}
