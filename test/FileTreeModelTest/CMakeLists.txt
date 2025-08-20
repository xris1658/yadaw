add_subdirectory(FileTreeModelTestContent)

add_executable(FileTreeModelTest
               ../../content/model/IFileTreeModel.hpp
               ../../content/model/ISortFilterItemModel.hpp
               ../../src/model/impl/TreeNode.hpp
               ../../src/model/FileTreeModel.cpp
               ../../src/model/FileTreeModel.hpp
               main.cpp
)

target_link_libraries(FileTreeModelTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                      FileTreeModelTestContent)