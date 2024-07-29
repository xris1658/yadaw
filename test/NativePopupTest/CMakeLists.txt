add_subdirectory(NativePopupTestContent)

if(WIN32)
    set(NATIVE_POPUP_TEST_NATIVE_SOURCE
        ../../src/native/win/Native.cpp
        ../../src/native/Native.hpp
    )
endif()

qt_add_executable(NativePopupTest
                  ../../src/ui/NativePopupEventFilter.cpp
                  ../../src/ui/NativePopupEventFilter.hpp
                  ../../src/ui/UI.cpp
                  ../../src/ui/UI.hpp
                  ${NATIVE_POPUP_TEST_NATIVE_SOURCE}
                  main.cpp
                  EventHandler.cpp
                  EventHandler.hpp)

if(WIN32)
    set(NATIVE_POPUP_TEST_NATIVE_LIBRARY version)
elseif(LINUX)
    set(NATIVE_POPUP_TEST_NATIVE_LIBRARY xcb xcb-icccm)
endif()

target_link_libraries(NativePopupTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick NativePopupTestContent
                      ${NATIVE_POPUP_TEST_NATIVE_LIBRARY})