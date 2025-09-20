#include "EventHandler.hpp"
#include "SimpleTreeModel.hpp"

#include "model/FileTreeModel.hpp"
#include "model/TreeModelToListModel.hpp"
#include "util/QmlUtil.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>


int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    using YADAW::Model::TreeModelToListModel;
    QML_REGISTER_TYPE(TreeModelToListModel, Models, 1, 0);
    QQuickWindow* window = nullptr;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        [&window](QObject* object, const QUrl&)
        {
            window = qobject_cast<QQuickWindow*>(object);
        }
    );
    engine.loadFromModule("TreeViewTestContent", "MainWindow");
    SimpleTreeModel stm;
    window->setProperty(
        "model", QVariant::fromValue<QObject*>(&stm)
    );
    EventHandler eh(stm);
    QObject::connect(window, SIGNAL(modifyModel()),
        &eh, SLOT(onModifyModel()));
    auto proxyModel = window->property("treeModelToListModel").value<QObject*>();
    return app.exec();
}
