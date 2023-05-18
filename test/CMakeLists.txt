if(WIN32)
    add_executable(AudioGraphTest
                   ../src/audio/backend/AudioGraphBackend.hpp
                   ../src/native/win/AudioGraphBackend.cpp
                   ../src/native/win/AudioGraphBackendImpl.cpp
                   ../src/native/win/AudioGraphBackendImpl.hpp
                   ../src/native/win/Native.cpp
                   ../src/native/Native.hpp
                   ../src/native/win/winrt/Forward.hpp
                   ../src/native/win/winrt/QStringFromHString.cpp
                   ../src/native/win/winrt/QStringFromHString.hpp
                   AudioGraphTest/main.cpp)
    target_precompile_headers(AudioGraphTest PRIVATE ../pch/WindowsAPI.hpp ../pch/QtCore.hpp)
    target_link_libraries(AudioGraphTest PRIVATE
                          Qt6::Core
                          version
                          windowsapp)
endif()

add_executable(LockFreeQueueTest
               ../src/util/CompilerSpecifics.hpp
               ../src/util/LockFreeQueue.hpp
               ../src/util/FixedSizeLockFreeQueue.hpp
               LockFreeQueueTest/main.cpp)

add_executable(CircularDequeTest
               ../src/util/CircularDeque.hpp
               ../src/util/FixedSizeCircularDeque.hpp
               CircularDequeTest/main.cpp)

add_executable(QStringConversionTest
               QStringConversionTest/main.cpp)

target_link_libraries(QStringConversionTest PRIVATE Qt6::Core)