#include "NativePopupEventFilterModel.hpp"

#include "ui/UI.hpp"
#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
NativePopupEventFilterModel::NativePopupEventFilterModel(QWindow& parentWindow):
    INativePopupEventFilterModel(&parentWindow),
    nativePopupEventFilter_(parentWindow)
{}

NativePopupEventFilterModel* NativePopupEventFilterModel::create(QWindow& parentWindow)
{
    return new NativePopupEventFilterModel(parentWindow);
}

NativePopupEventFilterModel::~NativePopupEventFilterModel()
{}

std::uint32_t NativePopupEventFilterModel::itemCount() const
{
    return nativePopupEventFilter_.count();
}

int NativePopupEventFilterModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant NativePopupEventFilterModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::NativePopup:
        {
            return QVariant::fromValue<QObject*>(
                const_cast<QWindow*>(&nativePopupEventFilter_[row])
            );
        }
        }
    }
    return {};
}

bool NativePopupEventFilterModel::insert(QWindow* nativePopup, int index, bool shouldReceiveKeyEvents)
{
    auto ret = false;
    if(nativePopup && index >= 0)
    {
        ret = nativePopupEventFilter_.insert(*nativePopup, index, shouldReceiveKeyEvents);
        if(ret)
        {
            beginInsertRows(QModelIndex(), index, index);
            endInsertRows();
        }
    }
    return ret;
}

bool NativePopupEventFilterModel::append(QWindow* nativePopup, bool shouldReceiveKeyEvents)
{
    auto ret = false;
    if(nativePopup)
    {
        beginInsertRows(QModelIndex(), itemCount(), itemCount());
        nativePopupEventFilter_.append(*nativePopup, shouldReceiveKeyEvents);
        endInsertRows();
        ret = true;
    }
    return ret;
}

void NativePopupEventFilterModel::popupShouldReceiveKeyEvents(
    QWindow* nativePopup, bool shouldReceiveKeyEvents)
{
    auto ret = false;
    if(nativePopup)
    {
        FOR_RANGE0(i, nativePopupEventFilter_.count())
        {
            if(&nativePopupEventFilter_[i] == nativePopup)
            {
                nativePopupEventFilter_.popupShouldReceiveKeyEvents(i, shouldReceiveKeyEvents);
                dataChanged(this->index(i), this->index(i), {Role::ShouldReceiveKeyEvents});
            }
        }
    }
}

bool NativePopupEventFilterModel::remove(QWindow* nativePopup)
{
    auto ret = false;
    FOR_RANGE0(i, itemCount())
    {
        if(&nativePopupEventFilter_[i] == nativePopup)
        {
            ret = true;
            beginRemoveRows(QModelIndex(), i, i);
            nativePopupEventFilter_.remove(i, 1);
            endRemoveRows();
        }
    }
    return ret;
}

void NativePopupEventFilterModel::clear()
{
    if(auto count = itemCount(); count > 0)
    {
        beginRemoveRows(QModelIndex(), 0, count - 1);
        nativePopupEventFilter_.clear();
        endRemoveRows();
    }
}
}
