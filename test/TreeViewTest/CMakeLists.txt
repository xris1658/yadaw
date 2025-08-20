add_subdirectory(TreeViewTestContent)

add_executable(TreeViewTest
               ../../content/model/TreeModelToListModel.hpp
               ../../src/model/TreeModelToListModel.cpp
               ../../content/model/IFileTreeModel.hpp
               ../../src/model/FileTreeModel.cpp
               ../../src/model/FileTreeModel.hpp
               ../../content/model/ISortFilterItemModel.hpp
               main.cpp)

target_link_libraries(TreeViewTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                      TreeViewTestContent)

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/tree)
set(DIR_NAMES 1 2 3 4 5)
foreach(X IN LISTS DIR_NAMES)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tree/${X})
    foreach(Y IN LISTS DIR_NAMES)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tree/${X}/${X}-${Y})
    endforeach()
endforeach()