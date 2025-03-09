#ifndef YADAW_SRC_MODEL_IMPL_CONNECTINSERTTOAUDIOIOPOSITION
#define YADAW_SRC_MODEL_IMPL_CONNECTINSERTTOAUDIOIOPOSITION

#include <QModelIndex>
#include <QObject>

#include <cstdint>

namespace YADAW::Model
{
class MixerAudioIOPositionItemModel;
class MixerChannelInsertListModel;
namespace Impl
{
class ConnectInsertToAudioIOPosition: public QObject
{
    Q_OBJECT
public:
    ConnectInsertToAudioIOPosition(
        MixerChannelInsertListModel& from,
        MixerAudioIOPositionItemModel& to,
        const QModelIndex& index
    );
public:
    void updateIndex(const QModelIndex& index);
private:
    MixerChannelInsertListModel* from_;
    MixerAudioIOPositionItemModel* to_;
    QModelIndex index_;
};
}
}

#endif // YADAW_SRC_MODEL_IMPL_CONNECTINSERTTOAUDIOIOPOSITION