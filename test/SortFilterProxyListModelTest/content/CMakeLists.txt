qt_add_library(SortFilterProxyListModelTestContent STATIC)

qt_add_qml_module(SortFilterProxyListModelTestContent
                  URI "SortFilterProxyListModelTestContent"
                  VERSION 1.0
                  QML_FILES
                  MainWindow.qml
                  RESOURCES
                  qmldir)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(SortFilterProxyListModelTestContent PRIVATE -g0)
endif()