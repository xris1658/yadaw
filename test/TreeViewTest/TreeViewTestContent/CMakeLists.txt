qt_add_library(TreeViewTestContent STATIC)

qt_add_qml_module(TreeViewTestContent
                  URI "TreeViewTestContent"
                  VERSION 1.0
                  NO_PLUGIN
                  QML_FILES MainWindow.qml
                  RESOURCES qmldir)