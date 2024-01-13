#include "AudioInsertListModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
AudioInsertListModel::AudioInsertListModel(Audio::Mixer::Inserts& inserts):
    IAudioInsertListModel(),
    inserts_(&inserts)
{
    auto& graph = inserts_->graph();
    auto insertCount = itemCount();
    inputs_.reserve(insertCount);
    outputs_.reserve(insertCount);
    FOR_RANGE0(i, insertCount)
    {
        inputs_.emplace_back(
            *(graph.getNodeData(*(inserts_->insertAt(i))).process.device()),
            true
        );
    }
    FOR_RANGE0(i, insertCount)
    {
        outputs_.emplace_back(
            *(graph.getNodeData(*(inserts_->insertAt(i))).process.device()),
            false
        );
    }
}

AudioInsertListModel::~AudioInsertListModel()
{}

int AudioInsertListModel::itemCount() const
{
    return inserts_->insertCount();
}

int AudioInsertListModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant AudioInsertListModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
            return QVariant::fromValue(inserts_->insertNameAt(row).value_or(QString()));
        case Role::AudioInputs:
            return QVariant::fromValue(&inputs_[row]);
        case Role::AudioOutputs:
            return QVariant::fromValue(&outputs_[row]);
        case Role::Bypassed:
            return QVariant::fromValue(inserts_->insertBypassed(row).value_or(false));
        case Role::Latency:
            return QVariant::fromValue(inserts_->insertLatencyAt(row).value_or(0));
        }
    }
    return {};
}

bool AudioInsertListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::Name:
        {
            inserts_->setName(row, value.value<QString>());
            dataChanged(this->index(row), this->index(row), {Role::Name});
            return true;
        }
        }
    }
    return false;
}
}