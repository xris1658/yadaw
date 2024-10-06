#include "QuickMenuBarEventFilterModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
QuickMenuBarEventFilterModel::QuickMenuBarEventFilterModel(QWindow& parentWindow, QObject& menuBar):
    INativePopupEventFilterModel(&parentWindow),
    quickMenuBarEventFilter_(parentWindow, menuBar)
{}

QuickMenuBarEventFilterModel* QuickMenuBarEventFilterModel::create(QWindow& parentWindow, QObject& menuBar)
{
    return new QuickMenuBarEventFilterModel(parentWindow, menuBar);
}

QuickMenuBarEventFilterModel::~QuickMenuBarEventFilterModel()
{
}

std::uint32_t QuickMenuBarEventFilterModel::itemCount() const
{
    return quickMenuBarEventFilter_.count();
}

int QuickMenuBarEventFilterModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant QuickMenuBarEventFilterModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < itemCount())
    {
        switch(role)
        {
        case Role::NativePopup:
        {
            return QVariant::fromValue<QObject*>(
                const_cast<QWindow*>(&quickMenuBarEventFilter_[row])
            );
        }
        }
    }
    return {};
}

bool QuickMenuBarEventFilterModel::insert(QWindow* nativePopup, int index, bool shouldReceiveKeyEvents)
{
    auto ret = false;
    if(nativePopup && index >= 0)
    {
        ret = quickMenuBarEventFilter_.insert(*nativePopup, index, shouldReceiveKeyEvents);
        if(ret)
        {
            beginInsertRows(QModelIndex(), index, index);
            endInsertRows();
        }
    }
    return ret;
}

bool QuickMenuBarEventFilterModel::append(QWindow* nativePopup, bool shouldReceiveKeyEvents)
{
    auto ret = false;
    if(nativePopup)
    {
        beginInsertRows(QModelIndex(), itemCount(), itemCount());
        quickMenuBarEventFilter_.append(*nativePopup, shouldReceiveKeyEvents);
        endInsertRows();
        ret = true;
    }
    return ret;
}

void QuickMenuBarEventFilterModel::popupShouldReceiveKeyEvents(QWindow* nativePopup, bool shouldReceiveKeyEvents)
{
    auto ret = false;
    if(nativePopup)
    {
        FOR_RANGE0(i, quickMenuBarEventFilter_.count())
        {
            if(&quickMenuBarEventFilter_[i] == nativePopup)
            {
                quickMenuBarEventFilter_.popupShouldReceiveKeyEvents(i, shouldReceiveKeyEvents);
                dataChanged(this->index(i), this->index(i), {Role::ShouldReceiveKeyEvents});
            }
        }
    }
}

bool QuickMenuBarEventFilterModel::remove(QWindow* nativePopup)
{
    auto ret = false;
    for(auto i = 0U; i < itemCount(); ++i)
    {
        if(&quickMenuBarEventFilter_[i] == nativePopup)
        {
            ret = true;
            beginRemoveRows(QModelIndex(), i, i);
            quickMenuBarEventFilter_.remove(i, 1);
            endRemoveRows();
        }
    }
    return ret;
}

void QuickMenuBarEventFilterModel::clear()
{
    if(auto count = itemCount(); count > 0)
    {
        beginRemoveRows(QModelIndex(), 0, count - 1);
        quickMenuBarEventFilter_.clear();
        endRemoveRows();
    }
}
}
