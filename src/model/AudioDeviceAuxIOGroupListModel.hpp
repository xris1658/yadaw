#ifndef YADAW_SRC_MODEL_AUDIODEVICEAUXIOGROUPLISTMODEL
#define YADAW_SRC_MODEL_AUDIODEVICEAUXIOGROUPLISTMODEL

#include "model/IAudioDeviceIOGroupListModel.hpp"

namespace YADAW::Model
{
class AudioDeviceAuxIOGroupListModel: public IAudioDeviceIOGroupListModel
{
    Q_OBJECT
public:
    AudioDeviceAuxIOGroupListModel(
        const IAudioDeviceIOGroupListModel& sourceModel,
        std::uint32_t excludeIndex,
        QObject* parent = nullptr);
    ~AudioDeviceAuxIOGroupListModel();
public:
    int itemCount() const;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
private:
    const IAudioDeviceIOGroupListModel* sourceModel_ = nullptr;
    std::uint32_t excludeIndex_;
};
}

#endif // YADAW_SRC_MODEL_AUDIODEVICEAUXIOGROUPLISTMODEL