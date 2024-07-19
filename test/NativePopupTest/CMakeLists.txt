add_subdirectory(NativePopupTestContent)

qt_add_executable(NativePopupTest
                  ../../src/ui/NativePopupEventFilter.cpp
                  ../../src/ui/NativePopupEventFilter.hpp
                  main.cpp
                  EventHandler.cpp
                  EventHandler.hpp)

target_link_libraries(NativePopupTest PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick NativePopupTestContent)