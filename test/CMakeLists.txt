if(WIN32)
    add_executable(AudioGraphTest
                   ../src/audio/backend/AudioGraphBackend.hpp
                   ../src/audio/backend/AudioGraphBusConfiguration.cpp
                   ../src/audio/backend/AudioGraphBusConfiguration.hpp
                   ../src/audio/device/IAudioChannelGroup.cpp
                   ../src/audio/device/IAudioChannelGroup.hpp
                   ../src/audio/engine/AudioDeviceGraph.cpp
                   ../src/audio/engine/AudioDeviceGraph.hpp
                   ../src/audio/engine/AudioProcessDataBufferContainer.hpp
                   ../src/audio/util/AudioBufferPool.cpp
                   ../src/audio/util/AudioBufferPool.hpp
                   ../src/audio/util/AudioChannelGroup.cpp
                   ../src/audio/util/AudioChannelGroup.hpp
                   ../src/audio/util/AudioProcessDataPointerContainer.hpp
                   ../src/audio/util/SampleDelay.cpp
                   ../src/audio/util/SampleDelay.hpp
                   ../src/audio/util/Summing.cpp
                   ../src/audio/util/Summing.hpp
                   ../src/native/win/AudioGraphBackend.cpp
                   ../src/native/win/AudioGraphBackendImpl.cpp
                   ../src/native/win/AudioGraphBackendImpl.hpp
                   ../src/native/win/Native.cpp
                   ../src/native/Native.hpp
                   ../src/native/win/winrt/Forward.hpp
                   ../src/native/win/winrt/QStringFromHString.cpp
                   ../src/native/win/winrt/QStringFromHString.hpp
                   ../src/util/ADEUtil.cpp
                   ../src/util/ADEUtil.hpp
                   ../src/util/IntegerRange.hpp
                   ../src/util/Util.cpp
                   ../src/util/Util.hpp
                   AudioGraphTest/main.cpp)
    target_precompile_headers(AudioGraphTest PRIVATE ../pch/WindowsAPI.hpp ../pch/QtCore.hpp)
    target_link_libraries(AudioGraphTest PRIVATE
                          ade
                          Qt6::Core
                          version
                          windowsapp)
endif()

if(LINUX)
    add_executable(ALSATest
                   ../src/audio/backend/ALSADeviceSelector.hpp
                   ../src/audio/backend/ALSABackend.hpp
                   ../src/audio/backend/ALSABusConfiguration.cpp
                   ../src/audio/backend/ALSABusConfiguration.hpp
                   ../src/audio/device/IAudioChannelGroup.cpp
                   ../src/audio/device/IAudioChannelGroup.hpp
                   ../src/audio/engine/AudioDeviceGraph.cpp
                   ../src/audio/engine/AudioDeviceGraph.hpp
                   ../src/audio/util/AudioBufferPool.cpp
                   ../src/audio/util/AudioBufferPool.hpp
                   ../src/audio/util/AudioChannelGroup.cpp
                   ../src/audio/util/AudioChannelGroup.hpp
                   ../src/audio/util/SampleDelay.cpp
                   ../src/audio/util/SampleDelay.hpp
                   ../src/native/linux/ALSABackend.cpp
                   ../src/native/linux/ALSABackendImpl.cpp
                   ../src/native/linux/ALSABackendImpl.hpp
                   ../src/native/linux/ALSADeviceEnumerator.cpp
                   ../src/native/linux/ALSADeviceEnumerator.hpp
                   ../src/native/linux/Sequencer.cpp
                   ../src/native/linux/Sequencer.hpp
                   ../src/util/ADEUtil.cpp
                   ../src/util/ADEUtil.hpp
                   ../src/util/SampleFormat.hpp
                   ALSATest/main.cpp
                   )
    target_precompile_headers(ALSATest PRIVATE ../pch/LinuxAPI.hpp ../pch/QtCore.hpp)
    target_link_libraries(ALSATest PRIVATE
                          ade
                          Qt6::Core
                          asound)
endif()

add_executable(ADEUtilsTest
    ../src/util/Util.cpp
    ../src/util/Util.hpp
    ../src/util/ADEUtil.cpp
    ../src/util/ADEUtil.hpp
    ADEUtilsTest/main.cpp)

add_executable(AudioBufferPoolTest
               ../src/audio/util/AudioBufferPool.cpp
               ../src/audio/util/AudioBufferPool.hpp
               AudioBufferPoolTest/main.cpp)

add_executable(AudioDeviceGraphTest
               ../src/audio/device/IAudioChannelGroup.cpp
               ../src/audio/device/IAudioChannelGroup.hpp
               ../src/audio/device/IAudioDevice.hpp
               ../src/audio/engine/AudioDeviceGraph.cpp
               ../src/audio/engine/AudioDeviceGraph.hpp
               ../src/audio/util/AudioChannelGroup.cpp
               ../src/audio/util/AudioChannelGroup.hpp
               ../src/audio/util/SampleDelay.cpp
               ../src/audio/util/SampleDelay.hpp
               ../src/audio/util/Summing.cpp
               ../src/audio/util/Summing.hpp
               ../src/util/ADEUtil.cpp
               ../src/util/ADEUtil.hpp
               AudioDeviceGraphTest/main.cpp)

add_executable(AudioProcessDataBufferContainerTest
               ../src/audio/util/AudioBufferPool.cpp
               ../src/audio/util/AudioBufferPool.hpp
               ../src/audio/util/AudioProcessDataPointerContainer.hpp
               ../src/audio/engine/AudioProcessDataBufferContainer.hpp
               AudioProcessDataBufferContainerTest/main.cpp)

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

add_executable(SampleDelayTest
               ../src/audio/util/SampleDelay.cpp
               ../src/audio/util/SampleDelay.hpp
               ../src/audio/util/AudioChannelGroup.cpp
               ../src/audio/util/AudioChannelGroup.hpp
               ../src/audio/device/IAudioChannelGroup.cpp
               ../src/audio/device/IAudioChannelGroup.hpp
               SampleDelayTest/main.cpp)

add_executable(ZeckendorfLogarithmTest ZeckendorfLogarithmTest/main.cpp)

target_link_libraries(ADEUtilsTest PRIVATE ade)

target_link_libraries(AudioDeviceGraphTest PRIVATE
                      ade
                      Qt6::Core)

target_link_libraries(AudioProcessDataBufferContainerTest PRIVATE Qt6::Core)

target_link_libraries(QStringConversionTest PRIVATE Qt6::Core)

target_link_libraries(SampleDelayTest PRIVATE Qt6::Core)
