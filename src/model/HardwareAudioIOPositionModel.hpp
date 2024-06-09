#ifndef YADAW_SRC_MODEL_HARDWAREAUDIOIOPOSITIONMODEL
#define YADAW_SRC_MODEL_HARDWAREAUDIOIOPOSITIONMODEL

#include "MixerChannelListModel.hpp"
#include "model/IAudioIOPositionModel.hpp"

namespace YADAW::Model
{
class HardwareAudioIOPositionModel: public IAudioIOPositionModel
{
    Q_OBJECT
public:
    HardwareAudioIOPositionModel(
        YADAW::Model::MixerChannelListModel& audioHardwareIOModel,
        QObject* parent = nullptr);
    ~HardwareAudioIOPositionModel() override;
public:
    int itemCount() const;
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    int findIndexByID(const QString& id) const override;
public:
    bool isComparable(int roleIndex) const override;
    bool isFilterable(int roleIndex) const override;
    bool isSearchable(int roleIndex) const override;
    bool isLess(int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const override;
    bool isSearchPassed(int roleIndex, const QModelIndex& modelIndex, const QString& string,
        Qt::CaseSensitivity caseSensitivity) const override;
    bool isPassed(const QModelIndex& modelIndex, int role, const QVariant& variant) const override;
private slots:
    void onSourceModelRowsAboutToBeInserted(
        const QModelIndex& parent, int start, int end
    );
    void onSourceModelRowsInserted(
        const QModelIndex& parent, int first, int last
    );
    void onSourceModelRowsAboutToBeRemoved(
        const QModelIndex& parent, int first, int last
    );
    void onSourceModelRowsRemoved(
        const QModelIndex& parent, int first, int last
    );
    void onSourceModelDataChanged(
        const QModelIndex& topLeft, const QModelIndex& bottomRight,
        const QList<int>& roles
    );
private:
    YADAW::Model::MixerChannelListModel* audioHardwareIOModel_;
};
}

#endif // YADAW_SRC_MODEL_HARDWAREAUDIOIOPOSITIONMODEL
