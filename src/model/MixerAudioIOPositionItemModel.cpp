#include "MixerAudioIOPositionItemModel.hpp"

#include "model/MixerChannelInsertListModel.hpp"

// |  #  | node                 | ptr |  0  |  1  | row | hasChildren |
// | --- | :------------------- | --- | --- | --- | --- | ----------- |
// |  0  | `v channel-0       ` | N/A |     |     |  0  |      1      |
// |  1  | `|--v instrument   ` |  1  |     |     |  0  |      1      |
// |  2  | `|  |-- input 0    ` |  2  |  0  |     |  0  |      0      |
// |  3  | `|  v pre-fader    ` |  1  |     |     |  1  |      1      |
// |  4  | `|  |--v insert 0  ` |  3  |  1  |     |  0  |      1      |
// |  5  | `|  |  '-- input 0 ` |  4  |  1  |  0  |  0  |      0      |
// |  6  | `|  |--v insert 1  ` |  3  |  1  |     |  1  |      1      |
// |  7  | `|  |--v insert 2  ` |  3  |  1  |     |  2  |      1      |
// |  8  | `|  |--v insert 3  ` |  3  |  1  |     |  3  |      1      |
// |  9  | `|  v post-fader   ` |  1  |     |     |  2  |      1      |
// |  10 | `|  |--v insert 0  ` |  9  |  2  |     |  0  |      1      |
// |  11 | `|  |  '-- input 0 ` |  10 |  2  |  0  |  0  |      0      |
// |  12 | `|  |--v insert 1  ` |  9  |  2  |     |  1  |      1      |
// |  13 | `|  |--v insert 2  ` |  9  |  2  |     |  2  |      1      |
// |  14 | `|  '--v insert 3  ` |  9  |  2  |     |  3  |      1      |
// |  15 | `v channel-1       ` | N/A |     |     |  1  |      1      |
// |  16 | `v channel-2       ` | N/A |     |     |  2  |      1      |
// |  17 | `v channel-3       ` | N/A |     |     |  3  |      1      |
// |  18 | `v channel-4       ` | N/A |     |     |  4  |      1      |
// |  19 | `v channel-5       ` | N/A |     |     |  5  |      1      |
// |  20 | `v channel-6       ` | N/A |     |     |  6  |      1      |
// |  21 | `v channel-7       ` | N/A |     |     |  7  |      1      |

namespace YADAW::Model
{
MixerAudioIOPositionItemModel::MixerAudioIOPositionItemModel(
        MixerChannelListModel& mixerAudioInputChannelListModel,
        MixerChannelListModel& mixerRegularChannelListModel,
        MixerChannelListModel& mixerAudioOutputChannelListModel,
        bool isInput):
    mixerAudioInputChannelListModel_(&mixerAudioInputChannelListModel),
    mixerRegularChannelListModel_(&mixerRegularChannelListModel),
    mixerAudioOutputChannelListModel_(&mixerAudioOutputChannelListModel),
    isInput_(isInput)
{}

// Check field from `parent`:     `internalPointer()`
// Fill  field to   return value: `internalPointer()`, `row()`
QModelIndex MixerAudioIOPositionItemModel::index(
    int row, int column, const QModelIndex& parent) const
{}

// Check field from `child`:      `internalPointer()`
// Fill  field to   return value: `internalPointer()`, `row()`
QModelIndex MixerAudioIOPositionItemModel::parent(const QModelIndex& child) const
{}

// Check child from `parent`: `internalPointer()`, `row()`
bool MixerAudioIOPositionItemModel::hasChildren(const QModelIndex& parent) const
{}

// Check child from `parent`: `internalPointer()`, `row()`
int MixerAudioIOPositionItemModel::rowCount(const QModelIndex& parent) const
{}

QVariant MixerAudioIOPositionItemModel::data(const QModelIndex& index, int role) const
{}

bool MixerAudioIOPositionItemModel::isComparable(int roleIndex) const
{}

bool MixerAudioIOPositionItemModel::isFilterable(int roleIndex) const
{}

bool MixerAudioIOPositionItemModel::isSearchable(int roleIndex) const
{}

QList<int> MixerAudioIOPositionItemModel::treeNodeRoles() const
{}

QList<int> MixerAudioIOPositionItemModel::leafNodeRoles() const
{}

bool MixerAudioIOPositionItemModel::isTreeNode(const QModelIndex& index) const
{}

bool MixerAudioIOPositionItemModel::isLess(
    int roleIndex, const QModelIndex& lhs, const QModelIndex& rhs) const
{}

bool MixerAudioIOPositionItemModel::isSearchPassed(
    int roleIndex, const QModelIndex& modelIndex, const QString& string,
    Qt::CaseSensitivity caseSensitivity) const
{}

bool MixerAudioIOPositionItemModel::isPassed(const QModelIndex& modelIndex,
    int role, const QVariant& variant) const
{}

QModelIndex MixerAudioIOPositionItemModel::findIndexByID(const QString& id) const
{}

std::unique_ptr<MixerAudioIOPositionItemModel::NodeData>
MixerAudioIOPositionItemModel::createRootNodeData(
    std::uint32_t index)
{}

void MixerAudioIOPositionItemModel::onSourceModelRowsInserted(
    const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onSourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onSourceModelRowsRemoved(const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onSourceModelDataChanged(
    const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{}

void MixerAudioIOPositionItemModel::onInsertModelRowsInserted(
    std::uint32_t index, const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onInsertModelRowsAboutToBeRemoved(
    std::uint32_t index, const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onInsertModelRowsRemoved(
    std::uint32_t index, const QModelIndex& parent, int first, int last)
{}

void MixerAudioIOPositionItemModel::onInsertModelDataChanged(
    std::uint32_t index, const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles)
{}
}
