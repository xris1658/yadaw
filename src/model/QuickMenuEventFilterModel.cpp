#include "QuickMenuEventFilterModel.hpp"

#include "util/IntegerRange.hpp"

namespace YADAW::Model
{
QuickMenuEventFilterModel::QuickMenuEventFilterModel(QWindow& parentWindow):
    INativePopupEventFilterModel(&parentWindow),
    quickMenuBarEventFilter_(parentWindow)
{}

QuickMenuEventFilterModel::QuickMenuEventFilterModel(QWindow& parentWindow, QObject& menuBar):
    INativePopupEventFilterModel(&parentWindow),
    quickMenuBarEventFilter_(parentWindow, menuBar)
{}

QuickMenuEventFilterModel* QuickMenuEventFilterModel::create(QWindow& parentWindow)
{
    return new QuickMenuEventFilterModel(parentWindow);
}

QuickMenuEventFilterModel* QuickMenuEventFilterModel::create(QWindow& parentWindow, QObject& menuBar)
{
    return new QuickMenuEventFilterModel(parentWindow, menuBar);
}

QuickMenuEventFilterModel::~QuickMenuEventFilterModel()
{
}

std::uint32_t QuickMenuEventFilterModel::itemCount() const
{
    return quickMenuBarEventFilter_.count();
}

int QuickMenuEventFilterModel::rowCount(const QModelIndex&) const
{
    return itemCount();
}

QVariant QuickMenuEventFilterModel::data(const QModelIndex& index, int role) const
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

bool QuickMenuEventFilterModel::insert(QWindow* nativePopup, int index, bool shouldReceiveKeyEvents)
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

bool QuickMenuEventFilterModel::append(QWindow* nativePopup, bool shouldReceiveKeyEvents)
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

void QuickMenuEventFilterModel::popupShouldReceiveKeyEvents(QWindow* nativePopup, bool shouldReceiveKeyEvents)
{
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

bool QuickMenuEventFilterModel::remove(QWindow* nativePopup)
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

void QuickMenuEventFilterModel::clear()
{
    if(auto count = itemCount(); count > 0)
    {
        beginRemoveRows(QModelIndex(), 0, count - 1);
        quickMenuBarEventFilter_.clear();
        endRemoveRows();
    }
}
}
