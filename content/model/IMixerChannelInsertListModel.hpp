#ifndef YADAW_CONTENT_MODEL_IMIXERCHANNELINSERTLISTMODEL
#define YADAW_CONTENT_MODEL_IMIXERCHANNELINSERTLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>

namespace YADAW::Model
{
class IMixerChannelInsertListModel: public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role
    {
        Name = Qt::UserRole,
        Bypassed,
        AudioInputs,
        AudioOutputs,
        EventInputs,
        EventOutputs,
        HasUI,
        WindowVisible,
        GenericEditorVisible,
        Latency,
        RoleCount
    };
public:
    IMixerChannelInsertListModel(QObject* parent = nullptr): QAbstractListModel(parent) {}
    virtual ~IMixerChannelInsertListModel() {}
public:
    static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { return columnCount(); }
public:
    Q_INVOKABLE virtual bool insert(int position, int pluginId) = 0;
    Q_INVOKABLE virtual bool append(int pluginId) = 0;
    Q_INVOKABLE virtual bool replace(int pluginId) = 0;
    Q_INVOKABLE virtual bool remove(int position, int removeCount) = 0;
    Q_INVOKABLE virtual void clear() = 0;
protected:
    RoleNames roleNames() const override
    {
        static RoleNames ret
        {
            std::make_pair(Role::Name, "mcilm_name"),
            std::make_pair(Role::Bypassed, "mcilm_bypassed"),
            std::make_pair(Role::AudioInputs, "mcilm_audio_inputs"),
            std::make_pair(Role::AudioOutputs, "mcilm_audio_outputs"),
            std::make_pair(Role::EventInputs, "mcilm_event_inputs"),
            std::make_pair(Role::EventOutputs, "mcilm_event_outputs"),
            std::make_pair(Role::HasUI, "mcilm_has_ui"),
            std::make_pair(Role::WindowVisible, "mcilm_window_visible"),
            std::make_pair(Role::GenericEditorVisible, "mcilm_generic_editor_visible"),
            std::make_pair(Role::Latency, "mcilm_latency")
        };
        return ret;
    }
};
}

#endif // YADAW_CONTENT_MODEL_IMIXERCHANNELINSERTLISTMODEL
