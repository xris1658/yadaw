qt_add_library(NativePopupTestContent STATIC)

qt_add_qml_module(NativePopupTestContent
                  URI "NativePopupTestContent"
                  NO_PLUGIN
                  VERSION 1.0
                  QML_FILES MainWindow.qml
                  RESOURCES qmldir
)