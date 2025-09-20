#include "EventHandler.hpp"

EventHandler::EventHandler(SimpleTreeModel& model, QObject* parent):
    QObject(parent), stm_(model)
{
}

EventHandler::~EventHandler()
{
}

void EventHandler::onModifyModel()
{
    if(index_ == 0)
    {
        stm_.insertRow(0);
        stm_.setData(stm_.index(0, 0, QModelIndex()), 2, SimpleTreeModel::Role::Data);
        stm_.insertRow(1);
        stm_.setData(stm_.index(1, 0, QModelIndex()), 1, SimpleTreeModel::Role::Data);
        stm_.insertRow(2);
        stm_.setData(stm_.index(2, 0, QModelIndex()), 7, SimpleTreeModel::Role::Data);
        stm_.insertRow(3);
        stm_.setData(stm_.index(3, 0, QModelIndex()), 9, SimpleTreeModel::Role::Data);
        stm_.insertRow(4);
        stm_.setData(stm_.index(4, 0, QModelIndex()), 8, SimpleTreeModel::Role::Data);
    }
    else if(index_ == 1)
    {
        stm_.insertRow(0, stm_.index(4, 0, QModelIndex()));
        stm_.setData(stm_.index(0, 0, stm_.index(4, 0, QModelIndex())), 3, SimpleTreeModel::Role::Data);
        stm_.insertRow(0, stm_.index(0, 0, stm_.index(4, 0, QModelIndex())));
        stm_.setData(stm_.index(0, 0, stm_.index(0, 0, stm_.index(4, 0, QModelIndex()))), 4, SimpleTreeModel::Role::Data);
    }
    else if(index_ == 2)
    {
        stm_.insertRow(1, stm_.index(4, 0, QModelIndex()));
        stm_.setData(stm_.index(1, 0, stm_.index(4, 0, QModelIndex())), 5, SimpleTreeModel::Role::Data);
        stm_.insertRow(0, stm_.index(1, 0, stm_.index(4, 0, QModelIndex())));
        stm_.setData(stm_.index(0, 0, stm_.index(1, 0, stm_.index(4, 0, QModelIndex()))), 6, SimpleTreeModel::Role::Data);
    }
    else if(index_ == 3)
    {
        stm_.moveRow(QModelIndex(), 3, QModelIndex(), 5);
        stm_.moveRow(QModelIndex(), 1, QModelIndex(), 0);
    }
    else if(index_ == 4)
    {
        stm_.moveRows(stm_.index(3, 0, QModelIndex()), 0, 2, stm_.index(1, 0, QModelIndex()), 0);
    }
    else
    {
        return;
    }
    ++index_;
}
