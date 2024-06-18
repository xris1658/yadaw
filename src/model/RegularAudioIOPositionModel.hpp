#ifndef YADAW_SRC_MODEL_REGULARAUDIOIOPOSITIONMODEL
#define YADAW_SRC_MODEL_REGULARAUDIOIOPOSITIONMODEL

#include "model/IAudioIOPositionModel.hpp"
#include "model/IMixerChannelListModel.hpp"
#include "model/SortFilterProxyListModel.hpp"

namespace YADAW::Model
{
class RegularAudioIOPositionModel: public IAudioIOPositionModel
{
public:
    RegularAudioIOPositionModel(YADAW::Model::IMixerChannelListModel& model,
        YADAW::Model::IMixerChannelListModel::ChannelTypes type,
        QObject* parent = nullptr);
    ~RegularAudioIOPositionModel() override;
public:
    int itemCount() const;
    int rowCount(const QModelIndex&) const override;
    QVariant data(const QModelIndex& index, int role) const override;
public:
    int findIndexByID(const QString& id) const override;
public:
    bool isComparable(int roleIndex) const override;
    bool isSearchable(int roleIndex) const override;
    bool isFilterable(int roleIndex) const override;
    bool isLess(int roleIndex,
        const QModelIndex& lhs, const QModelIndex& rhs) const override;
    bool isPassed(const QModelIndex& modelIndex, int role,
        const QVariant& variant) const override;
    bool isSearchPassed(int roleIndex, const QModelIndex& modelIndex,
        const QString& string,
        Qt::CaseSensitivity caseSensitivity) const override;
public:
    void onSourceModelRowsAboutToBeInserted(
        const QModelIndex& parent, int start, int end);
    void onSourceModelRowsInserted(
        const QModelIndex& parent, int start, int end);
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
    YADAW::Model::SortFilterProxyListModel model_;
};
}

#endif // YADAW_SRC_MODEL_REGULARAUDIOIOPOSITIONMODEL
