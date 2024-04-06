add_subdirectory(FileTreeModelTestContent)

add_executable(FileTreeModelTest
               ../../src/model/impl/TreeNode.hpp
               ../../src/model/FileTreeModel.cpp
               ../../src/model/FileTreeModel.hpp
               main.cpp
)

target_link_libraries(FileTreeModelTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                      FileTreeModelTestContentplugin)