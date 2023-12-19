add_subdirectory(content)

add_executable(SortFilterProxyListModelTest
               ../../content/model/IFilterRoleModel.hpp
               ../../content/model/ISortOrderModel.hpp
               ../../content/model/ISortFilterListModel.hpp
               ../../content/model/ISortFilterProxyListModel.hpp
               ../../src/model/FilterRoleModel.cpp
               ../../src/model/FilterRoleModel.hpp
               ../../src/model/SortFilterProxyListModel.cpp
               ../../src/model/SortFilterProxyListModel.hpp
               ../../src/model/SortOrderModel.cpp
               ../../src/model/SortOrderModel.hpp
               main.cpp
               IntegerModel.cpp
               IntegerModel.hpp)

target_link_libraries(SortFilterProxyListModelTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick
                      SortFilterProxyListModelTestContentplugin)