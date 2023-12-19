#include "IntegerModel.hpp"

#include "model/SortFilterProxyListModel.hpp"
#include "util/QmlUtil.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <cstdio>

int main(int argc, char* argv[])
{
    using YADAW::Model::ISortFilterProxyListModel;
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QML_REGISTER_TYPE(IntegerModel, Models, 1, 0);
    QML_REGISTER_TYPE(ISortFilterProxyListModel, Models, 1, 0);
    QObject* object = nullptr;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [&](QObject* obj, const QUrl& objUrl)
        {
            if(obj)
            {
                object = obj;
            }
        }
    );
    engine.load(u"qrc:SortFilterProxyListModelTestContent/MainWindow.qml"_qs);
    IntegerModel model;
    YADAW::Model::SortFilterProxyListModel proxyModel(model);
    proxyModel.appendSortOrder(IntegerModel::Role::Value, Qt::SortOrder::AscendingOrder);
    object->setProperty("sourceModel", QVariant::fromValue<QObject*>(&model));
    object->setProperty("proxyModel", QVariant::fromValue<QObject*>(&proxyModel));
    QList<int> list;
    list.reserve(16);
    for(int i = 0; i < 16; ++i)
    {
        list.append((i * 3) % 16 + 1);
    }
    model.append(list);
    return app.exec();
}