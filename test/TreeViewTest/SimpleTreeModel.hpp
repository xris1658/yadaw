#ifndef YADAW_TEST_TREEVIEWTEST_SIMPLETREEMODEL
#define YADAW_TEST_TREEVIEWTEST_SIMPLETREEMODEL

#include <QAbstractItemModel>

#include <memory>
#include <vector>

class SimpleTreeModel: public QAbstractItemModel
{
    Q_OBJECT
private:
    struct Node
    {
        int data = 0;
        int row = 0;
        std::vector<std::unique_ptr<Node>> children;
        Node* parent = nullptr;
    };
public:
    enum Role { Data = Qt::UserRole };
public:
    SimpleTreeModel(QObject* parent = nullptr);
    ~SimpleTreeModel() override;
public:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    bool hasChildren(const QModelIndex& parent) const override;
public:
    bool insertRows(int row, int count, const QModelIndex& parent) override;
    bool removeRows(int row, int count, const QModelIndex& parent) override;
    bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild) override;
protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    const Node* getParentNode(const QModelIndex& index) const;
          Node* getParentNode(const QModelIndex& index);
    const Node* getNode      (const QModelIndex& index) const;
          Node* getNode      (const QModelIndex& index);
private:
    Node root_;
};

#endif // YADAW_TEST_TREEVIEWTEST_SIMPLETREEMODEL