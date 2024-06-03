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
        model.append((i * 3) % count + 1);
    }
    QTimer timer;
    timer.setInterval(1000);
    timer.setSingleShot(false);
    timer.callOnTimeout(
        [&timer, &model]()
        {
            model.remove(0, 1);
            if(model.itemCount() == 0)
            {
                QList<int> list;
                list.reserve(count);
                for(int i = 0; i < count; ++i)
                {
                    list.append((i * 3) % count + 1);
                }
                model.append(list);
                model.remove(0, count);
                timer.stop();
            }
        }
    );
    timer.start();
    return app.exec();
}