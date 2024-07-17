qt_add_library(FileTreeModelTestContent STATIC)

qt_add_qml_module(FileTreeModelTestContent
                  URI "FileTreeModelTestContent"
                  VERSION 1.0
                  NO_PLUGIN
                  QML_FILES MainWindow.qml
                  RESOURCES qmldir)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(FileTreeModelTestContent PRIVATE -g0)
endif()