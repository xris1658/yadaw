#include "SimpleTreeModel.hpp"

SimpleTreeModel::SimpleTreeModel(QObject* parent): QAbstractItemModel(parent)
{}

SimpleTreeModel::~SimpleTreeModel()
{}

int SimpleTreeModel::rowCount(const QModelIndex& parent) const
{
    return getNode(parent)->children.size();
}

int SimpleTreeModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant SimpleTreeModel::data(const QModelIndex& index, int role) const
{
    if(auto row = index.row(); row >= 0 && row < rowCount(parent(index))
        && role == Role::Data)
    {
        return getNode(index)->data;
    }
    return QVariant();
}

bool SimpleTreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(auto row = index.row(); row >= 0 && row < rowCount(parent(index)) // getParentNode(index)->children.size()
        && role == Role::Data)
    {
        getNode(index)->data = value.value<int>();
        dataChanged(index, index, {Role::Data});
        return true;
    }
    return false;
}

QModelIndex SimpleTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, getNode(parent));
}

QModelIndex SimpleTreeModel::parent(const QModelIndex& child) const
{
    auto node = getNode(child);
    if(node == nullptr || node->parent == nullptr || node->parent == &root_)
    {
        return QModelIndex();
    }
    auto parentNode = node->parent;
    return createIndex(parentNode->row, 0, parentNode->parent);
}

bool SimpleTreeModel::hasChildren(const QModelIndex& parent) const
{
    return true;
}

bool SimpleTreeModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if(count > 0)
    {
        beginInsertRows(parent, row, row + count - 1);
        auto parentNode = getNode(parent);
        auto rowIndex = row;
        std::generate_n(
            std::inserter(parentNode->children, parentNode->children.begin() + row), count,
            [parentNode, &rowIndex]() mutable
            {
                return std::make_unique<Node>(Node {.data = -1, .row = rowIndex++, .parent = parentNode});
            }
        );
        for(auto it = parentNode->children.begin() + row + count; it < parentNode->children.end(); ++it)
        {
            (*it)->row = rowIndex++;
        }
        endInsertRows();
        return true;
    }
    return false;
}

bool SimpleTreeModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if(count > 0)
    {
        beginRemoveRows(parent, row, row + count - 1);
        auto parentNode = getNode(parent);
        parentNode->children.erase(
            parentNode->children.begin() + row,
            parentNode->children.begin() + row + count
        );
        auto rowIndex = row;
        for(auto it = parentNode->children.begin() + row; it < parentNode->children.end(); ++it)
        {
            (*it)->row = rowIndex++;
        }
        endRemoveRows();
        return true;
    }
    return false;
}

bool SimpleTreeModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count,
    const QModelIndex& destinationParent, int destinationChild)
{
    if(count > 0 && beginMoveRows(sourceParent, sourceRow, sourceRow + count - 1, destinationParent, destinationChild))
    {
        if(sourceParent == destinationParent)
        {
            auto sourceParentNode = getNode(sourceParent);
            auto newFirst = sourceParentNode->children.begin();
            auto rowIndex = 0;
            if(sourceRow < destinationChild)
            {
                std::rotate(
                    sourceParentNode->children.begin() + sourceRow,
                    sourceParentNode->children.begin() + sourceRow + count,
                    sourceParentNode->children.begin() + destinationChild
                );
                rowIndex = sourceRow;
            }
            else
            {
                std::rotate(
                    sourceParentNode->children.begin() + destinationChild,
                    sourceParentNode->children.begin() + sourceRow,
                    sourceParentNode->children.begin() + sourceRow + count
                );
                rowIndex = destinationChild;
            }
            for(auto it = sourceParentNode->children.begin() + rowIndex; it != sourceParentNode->children.end(); ++it)
            {
                (*it)->row = rowIndex++;
            }
        }
        else
        {
            auto sourceParentNode = getNode(sourceParent);
            auto destParentNode = getNode(destinationParent);
            std::move(
                sourceParentNode->children.begin() + sourceRow,
                sourceParentNode->children.begin() + sourceRow + count,
                std::inserter(destParentNode->children, destParentNode->children.begin() + destinationChild)
            );
            sourceParentNode->children.erase(
                sourceParentNode->children.begin() + sourceRow,
                sourceParentNode->children.begin() + sourceRow + count
            );
            auto rowIndex = sourceRow;
            for(auto it = sourceParentNode->children.begin() + sourceRow; it < sourceParentNode->children.end(); ++it)
            {
                (*it)->row = rowIndex++;
            }
            rowIndex = destinationChild;
            for(auto it = destParentNode->children.begin() + destinationChild; it < destParentNode->children.end(); ++it)
            {
                (*it)->row = rowIndex++;
            }
        }
        endMoveRows();
    }
    return false;
}

QHash<int, QByteArray> SimpleTreeModel::roleNames() const
{
    auto ret = QAbstractItemModel::roleNames();
    ret.insert(Role::Data, "stm_data");
    return ret;
}

const SimpleTreeModel::Node* SimpleTreeModel::getParentNode(const QModelIndex& index) const
{
    return static_cast<const Node*>(index.internalPointer());
}

SimpleTreeModel::Node* SimpleTreeModel::getParentNode(const QModelIndex& index)
{
    return static_cast<Node*>(index.internalPointer());
}

const SimpleTreeModel::Node* SimpleTreeModel::getNode(const QModelIndex& index) const
{
    if(index == QModelIndex())
    {
        return &root_;
    }
    return getParentNode(index)->children[index.row()].get();
}

SimpleTreeModel::Node* SimpleTreeModel::getNode(const QModelIndex& index)
{
    return const_cast<Node*>(static_cast<const SimpleTreeModel*>(this)->getNode(index));
}
