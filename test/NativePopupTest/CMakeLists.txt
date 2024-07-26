add_subdirectory(NativePopupTestContent)

qt_add_executable(NativePopupTest
                  ../../src/ui/NativePopupEventFilter.cpp
                  ../../src/ui/NativePopupEventFilter.hpp
                  main.cpp
                  EventHandler.cpp
                  EventHandler.hpp)

if(LINUX)
    set(NATIVE_POPUP_TEST_NATIVE_LIBRARY xcb)
endif()

target_link_libraries(NativePopupTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick NativePopupTestContent
                      ${NATIVE_POPUP_TEST_NATIVE_LIBRARY})