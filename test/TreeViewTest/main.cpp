#include "model/FileTreeModel.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    QQuickWindow* window = nullptr;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        [&window](QObject* object, const QUrl&)
        {
            window = qobject_cast<QQuickWindow*>(object);
        }
    );
    engine.loadFromModule("TreeViewTestContent", "MainWindow");
    return app.exec();
}