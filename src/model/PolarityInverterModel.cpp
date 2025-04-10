#include "PolarityInverterModel.hpp"

#include "util/QmlUtil.hpp"

namespace YADAW::Model
{
PolarityInverterModel::PolarityInverterModel(
    YADAW::Audio::Mixer::PolarityInverter& polarityInverter, QObject* parent):
    IPolarityInverterModel(parent),
    polarityInverter_(&polarityInverter)
{
    YADAW::Util::setCppOwnership(*this);
}

PolarityInverterModel::~PolarityInverterModel()
{
}

int PolarityInverterModel::itemCount() const
{
    return polarityInverter_->audioInputGroupAt(0)->get().channelCount();
}

int PolarityInverterModel::rowCount(const QModelIndex& parent) const
{
    return itemCount();
}

QVariant PolarityInverterModel::data(const QModelIndex& index, int role) const
{
    auto row = index.row();
    if(row >= 0 && row < itemCount())
    {
        auto& group = polarityInverter_->audioInputGroupAt(0)->get();
        auto inverted = polarityInverter_->inverted();
        switch(role)
        {
        case ChannelName:
        {
            return QVariant::fromValue<QString>(group.speakerNameAt(row));
        }
        case Inverted:
        {
            return QVariant::fromValue<bool>(inverted & (1 << row));
        }
        }
    }
    return QVariant();
}

bool PolarityInverterModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    auto row = index.row();
    auto itemCount = this->itemCount();
    if(row >= 0 && row < itemCount)
    {
        switch(role)
        {
        case Inverted:
        {
            auto inverted = polarityInverter_->inverted();
            if(value.value<bool>())
            {
                inverted |= (1 << row);
            }
            else
            {
                inverted -= ((1 << row) & inverted);
            }
            polarityInverter_->setInverted(inverted);
            dataChanged(index, index, {Role::Inverted});
            return true;
        }
        }
    }
    return false;
}

bool PolarityInverterModel::toggle(int channelIndex)
{
    if(channelIndex >= 0 && channelIndex < itemCount())
    {
        auto inverted = polarityInverter_->inverted();
        inverted ^= (1 << channelIndex);
        polarityInverter_->setInverted(inverted);
        dataChanged(index(channelIndex), index(channelIndex), {Role::Inverted});
        return true;
    }
    return false;
}

void PolarityInverterModel::invertAll()
{
    polarityInverter_->setInverted((1 << itemCount()) - 1);
    dataChanged(index(0), index(itemCount() - 1), {Role::Inverted});
}

void PolarityInverterModel::revertAll()
{
    polarityInverter_->setInverted(0);
    dataChanged(index(0), index(itemCount() - 1), {Role::Inverted});
}

void PolarityInverterModel::toggleAll()
{
    auto inverted = polarityInverter_->inverted();
    inverted ^= ((1 << itemCount()) - 1);
    polarityInverter_->setInverted(inverted);
    dataChanged(index(0), index(itemCount() - 1), {Role::Inverted});
}
}
