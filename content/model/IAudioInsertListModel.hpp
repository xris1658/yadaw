#ifndef YADAW_CONTENT_MODEL_IAUDIOINSERTLISTMODEL
#define YADAW_CONTENT_MODEL_IAUDIOINSERTLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IAudioInsertListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        Bypassed,
        AudioInputs,
        AudioOutputs,
        Latency,
        RoleCount
    };
    IAudioInsertListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IAudioInsertListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name, "ailm_name"),
            std::make_pair(Role::Bypassed, "ailm_bypassed"),
            std::make_pair(Role::AudioInputs, "ailm_audio_inputs"),
            std::make_pair(Role::AudioOutputs, "ailm_audio_outputs"),
            std::make_pair(Role::Latency, "ailm_latency")
        };
        return ret;
    }
};
}

#endif //YADAW_CONTENT_MODEL_IAUDIOINSERTLISTMODEL
