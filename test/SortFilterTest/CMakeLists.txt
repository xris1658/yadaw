add_subdirectory(SortFilterTestContent)

add_executable(SortFilterTest
               ../../content/model/FilterRoleModel.hpp
               ../../content/model/ISortFilterListModel.hpp
               ../../content/model/SortFilterProxyListModel.hpp
               ../../content/model/SortOrderModel.hpp
               ../../src/model/FilterRoleModel.cpp
               ../../src/model/SortFilterProxyListModel.cpp
               ../../src/model/SortOrderModel.cpp
               main.cpp
               IntegerModel.cpp
               IntegerModel.hpp)

target_link_libraries(SortFilterTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                      SortFilterTestContentplugin)