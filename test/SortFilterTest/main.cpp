#include "IntegerModel.hpp"

#include "model/SortFilterProxyListModel.hpp"
#include "util/QmlUtil.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <cstdio>

int main(int argc, char* argv[])
{
    using YADAW::Model::SortFilterProxyListModel;
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QML_REGISTER_TYPE(IntegerModel, Models, 1, 0);
    QML_REGISTER_TYPE(SortFilterProxyListModel, Models, 1, 0);
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
    engine.loadFromModule("SortFilterTestContent", "MainWindow");
    IntegerModel model;
    object->setProperty("sourceModel", QVariant::fromValue<QObject*>(&model));
    QList<int> list;
    list.reserve(16);
    for(int i = 0; i < 16; ++i)
    {
        list.append((i * 3) % 16 + 1);
    }
    model.append(list);
    model.clear();
    return app.exec();
}