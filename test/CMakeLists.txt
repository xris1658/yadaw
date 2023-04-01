add_executable(AudioGraphTest
               ../src/audio/backend/AudioGraphBackend.hpp
               ../src/native/AudioGraphBackend.cpp
               ../src/native/AudioGraphBackendImpl.cpp
               ../src/native/AudioGraphBackendImpl.hpp
               ../src/native/Native.cpp
               ../src/native/Native.hpp
               ../src/native/winrt/Forward.hpp
               ../src/native/winrt/QStringFromHString.cpp
               ../src/native/winrt/QStringFromHString.hpp
               AudioGraphTest/main.cpp)

target_precompile_headers(AudioGraphTest PRIVATE ../pch/WindowsAPI.hpp ../pch/QtCore.hpp)

target_link_libraries(AudioGraphTest PRIVATE
                      Qt6::Core
                      version
                      windowsapp)

add_executable(LockFreeQueueTest
               ../src/util/CompilerSpecifics.hpp
               ../src/util/LockFreeQueue.hpp
               ../src/util/FixedSizeLockFreeQueue.hpp
               LockFreeQueueTest/main.cpp)

add_executable(CircularDequeTest
               ../src/util/CircularDeque.hpp
               ../src/util/FixedSizeCircularDeque.hpp
               CircularDequeTest/main.cpp)