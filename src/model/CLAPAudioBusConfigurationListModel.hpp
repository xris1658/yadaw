#ifndef YADAW_SRC_MODEL_CLAPAUDIOBUSCONFIGURATIONLISTMODEL
#define YADAW_SRC_MODEL_CLAPAUDIOBUSCONFIGURATIONLISTMODEL

#include "model/ICLAPAudioBusConfigurationListModel.hpp"

#include "audio/plugin/CLAPPlugin.hpp"

namespace YADAW::Model
{
class CLAPAudioBusConfigurationListModel: public ICLAPAudioBusConfigurationListModel
{
    Q_OBJECT
public:
    CLAPAudioBusConfigurationListModel(
        YADAW::Audio::Plugin::CLAPPlugin& plugin,
        QObject* parent = nullptr);
    ~CLAPAudioBusConfigurationListModel() override;
public:
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const override;
    bool isSearchPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string, Qt::CaseSensitivity) const override;
    bool isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const override;
private:
    YADAW::Audio::Plugin::CLAPPlugin* plugin_;
};
}

#endif // YADAW_SRC_MODEL_CLAPAUDIOBUSCONFIGURATIONLISTMODEL