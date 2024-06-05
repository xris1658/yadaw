#include "IntegerModel.hpp"

#include "model/SortFilterProxyListModel.hpp"
#include "util/QmlUtil.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTimer>

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
    constexpr auto count = 16;
    for(int i = 0; i < count; ++i)
    {
        model.append((i * 3) % count);
    }
    model.remove(4, 4);
    return app.exec();
}