qt_add_library(SortFilterTestContent STATIC)

qt_add_qml_module(SortFilterTestContent
                  URI "SortFilterTestContent"
                  VERSION 1.0
                  QML_FILES
                  MainWindow.qml
                  RESOURCES
                  qmldir)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_compile_options(SortFilterTestContent PRIVATE -g0)
endif()