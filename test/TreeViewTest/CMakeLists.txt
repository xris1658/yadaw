add_subdirectory(TreeViewTestContent)

add_executable(TreeViewTest
               ../../content/model/TreeModelToListModel.hpp
               ../../src/model/TreeModelToListModel.cpp
               EventHandler.cpp
               EventHandler.hpp
               SimpleTreeModel.cpp
               SimpleTreeModel.hpp
               main.cpp
)

target_link_libraries(TreeViewTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                      TreeViewTestContent)
