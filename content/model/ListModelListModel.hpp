#ifndef YADAW_CONTENT_MODEL_LISTMODELLISTMODEL
#define YADAW_CONTENT_MODEL_LISTMODELLISTMODEL

#include "ModelBase.hpp"

#include <QAbstractListModel>
#include <QMetaObject>
#include <QMetaType>

#include <array>
#include <vector>

namespace YADAW::Model
{
class ListModelListModel: public QAbstractListModel
{
	Q_OBJECT
public:
	enum Role
	{
		List = Qt::UserRole,
		RoleCount
	};
public:
	ListModelListModel(QObject* parent = nullptr);
	ListModelListModel(const ListModelListModel& rhs);
	~ListModelListModel() override;
public:
	int itemCount() const;
	int rowCount(const QModelIndex&) const override;
	int columnCount(const QModelIndex& parent) const override;
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;
public:
	Q_INVOKABLE bool insertList(QAbstractListModel* model, int position);
	Q_INVOKABLE bool appendList(QAbstractListModel* model);
	Q_INVOKABLE bool removeLists(int first, int last);
	Q_INVOKABLE bool removeList(int index);
	Q_INVOKABLE bool moveLists(int first, int last, int newFirst);
	Q_INVOKABLE bool moveList(int index, int newIndex);
private:
	std::vector<QAbstractListModel*> models_;
};
}

Q_DECLARE_METATYPE(YADAW::Model::ListModelListModel)

#endif // YADAW_CONTENT_MODEL_LISTMODELLISTMODEL