if(WIN32)
    add_executable(AudioGraphTest
                   ../src/audio/backend/AudioGraphBackend.hpp
                   ../src/audio/backend/AudioGraphBusConfiguration.cpp
                   ../src/audio/backend/AudioGraphBusConfiguration.hpp
                   ../src/audio/backend/AudioGraphErrorText.cpp
                   ../src/audio/backend/AudioGraphErrorText.hpp
                   ../src/audio/device/IAudioChannelGroup.cpp
                   ../src/audio/device/IAudioChannelGroup.hpp
                   ../src/audio/engine/AudioDeviceGraphBase.cpp
                   ../src/audio/engine/AudioDeviceGraphBase.hpp
                   ../src/audio/engine/AudioDeviceGraph.hpp
                   ../src/audio/engine/extension/Buffer.cpp
                   ../src/audio/engine/extension/Buffer.hpp
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
                   ../src/native/Library.hpp
                   ../src/native/win/Library.cpp
                   ../src/native/win/Native.cpp
                   ../src/native/Native.hpp
                   ../src/native/win/winrt/Forward.hpp
                   ../src/native/win/winrt/QStringFromHString.cpp
                   ../src/native/win/winrt/QStringFromHString.hpp
                   ../src/util/ADEUtil.cpp
                   ../src/util/ADEUtil.hpp
                   ../src/util/IntegerRange.hpp
                   ../src/util/IntrusivePointer.hpp
                   ../src/util/IntrusiveRefCounter.cpp
                   ../src/util/IntrusiveRefCounter.hpp
                   ../src/util/Util.cpp
                   ../src/util/Util.hpp
                   AudioGraphTest/main.cpp)
    if(${ENABLE_PRECOMPILE})
        target_precompile_headers(AudioGraphTest PRIVATE ../pch/WindowsAPI.hpp ../pch/QtCore.hpp)
    endif()
    target_link_libraries(AudioGraphTest PRIVATE
                          ade
                          Qt6::Core
                          version
                          windowsapp)

    add_executable(WASAPIExclusiveBackendTest
                   ../src/audio/backend/WASAPIExclusiveBackend.hpp
                   ../src/native/win/WASAPIExclusiveBackend.cpp
                   ../src/native/win/WASAPIExclusiveBackendImpl.cpp
                   ../src/native/win/WASAPIExclusiveBackendImpl.hpp
                   ../src/util/IntegerRange.hpp
                   WASAPIExclusiveBackendTest/main.cpp
    )
    target_link_libraries(WASAPIExclusiveBackendTest PRIVATE
                          Qt6::Core avrt
    )
endif()

if(LINUX)
    add_executable(ALSATest
                   ../src/audio/backend/ALSADeviceSelector.hpp
                   ../src/audio/backend/ALSABackend.hpp
                   ../src/audio/backend/ALSABusConfiguration.cpp
                   ../src/audio/backend/ALSABusConfiguration.hpp
                   ../src/audio/backend/ALSAErrorText.cpp
                   ../src/audio/backend/ALSAErrorText.hpp
                   ../src/audio/device/IAudioChannelGroup.cpp
                   ../src/audio/device/IAudioChannelGroup.hpp
                   ../src/audio/engine/AudioDeviceGraphBase.cpp
                   ../src/audio/engine/AudioDeviceGraphBase.hpp
                   ../src/audio/engine/AudioDeviceGraph.hpp
                   ../src/audio/engine/extension/Buffer.cpp
                   ../src/audio/engine/extension/Buffer.hpp
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
                   ../src/util/IntrusivePointer.hpp
                   ../src/util/IntrusiveRefCounter.cpp
                   ../src/util/IntrusiveRefCounter.hpp
                   ../src/util/SampleFormat.hpp
                   common/SineWaveGenerator.cpp
                   common/SineWaveGenerator.hpp
                   ALSATest/main.cpp
    )
    if(${ENABLE_PRECOMPILE})
        target_precompile_headers(ALSATest PRIVATE ../pch/LinuxAPI.hpp ../pch/QtCore.hpp)
    endif()
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
               ../src/util/IntrusivePointer.hpp
               ../src/util/IntrusiveRefCounter.cpp
               ../src/util/IntrusiveRefCounter.hpp
               ../src/audio/util/AudioBufferPool.cpp
               ../src/audio/util/AudioBufferPool.hpp
               AudioBufferPoolTest/main.cpp)

add_executable(AudioDeviceGraphTest
               ../src/audio/device/IAudioChannelGroup.cpp
               ../src/audio/device/IAudioChannelGroup.hpp
               ../src/audio/engine/AudioDeviceGraphBase.cpp
               ../src/audio/engine/AudioDeviceGraphBase.hpp
               ../src/audio/engine/AudioDeviceGraph.hpp
               ../src/audio/engine/AudioDeviceGraphWithPDC.cpp
               ../src/audio/engine/AudioDeviceGraphWithPDC.hpp
               ../src/audio/engine/MultiInputDeviceWithPDC.cpp
               ../src/audio/engine/MultiInputDeviceWithPDC.hpp
               ../src/audio/engine/extension/Buffer.cpp
               ../src/audio/engine/extension/Buffer.hpp
               ../src/audio/engine/extension/UpstreamLatency.cpp
               ../src/audio/engine/extension/UpstreamLatency.hpp
               ../src/audio/util/AudioChannelGroup.cpp
               ../src/audio/util/AudioChannelGroup.hpp
               ../src/audio/util/AudioBufferPool.cpp
               ../src/audio/util/AudioBufferPool.hpp
               ../src/audio/util/SampleDelay.cpp
               ../src/audio/util/SampleDelay.hpp
               ../src/audio/util/Summing.cpp
               ../src/audio/util/Summing.hpp
               ../src/util/ADEUtil.cpp
               ../src/util/ADEUtil.hpp
               ../src/util/IntrusivePointer.hpp
               ../src/util/IntrusiveRefCounter.cpp
               ../src/util/IntrusiveRefCounter.hpp
               ../src/util/Util.cpp
               ../src/util/Util.hpp
               common/SineWaveGenerator.cpp
               common/SineWaveGenerator.hpp
               AudioDeviceGraphTest/main.cpp)

add_executable(AudioDeviceGraphWithPDCTest
               ../src/audio/device/IAudioChannelGroup.cpp
               ../src/audio/device/IAudioChannelGroup.hpp
               ../src/audio/engine/AudioDeviceGraph.hpp
               ../src/audio/engine/AudioDeviceGraphBase.cpp
               ../src/audio/engine/AudioDeviceGraphBase.hpp
               ../src/audio/engine/AudioDeviceGraphWithPDC.cpp
               ../src/audio/engine/AudioDeviceGraphWithPDC.hpp
               ../src/audio/engine/AudioProcessDataBufferContainer.hpp
               ../src/audio/engine/MultiInputDeviceWithPDC.cpp
               ../src/audio/engine/MultiInputDeviceWithPDC.hpp
               ../src/audio/engine/extension/Buffer.cpp
               ../src/audio/engine/extension/Buffer.hpp
               ../src/audio/engine/extension/UpstreamLatency.cpp
               ../src/audio/engine/extension/UpstreamLatency.hpp
               ../src/audio/mixer/PolarityInverter.cpp
               ../src/audio/mixer/PolarityInverter.hpp
               ../src/audio/util/AudioBufferPool.cpp
               ../src/audio/util/AudioBufferPool.hpp
               ../src/audio/util/AudioChannelGroup.cpp
               ../src/audio/util/AudioChannelGroup.hpp
               ../src/audio/util/AudioProcessDataPointerContainer.hpp
               ../src/audio/util/SampleDelay.cpp
               ../src/audio/util/SampleDelay.hpp
               ../src/audio/util/Summing.cpp
               ../src/audio/util/Summing.hpp
               ../src/util/ADEUtil.cpp
               ../src/util/ADEUtil.hpp
               ../src/util/IntrusivePointer.hpp
               ../src/util/IntrusivePointer.hpp
               ../src/util/IntrusiveRefCounter.cpp
               ../src/util/IntrusiveRefCounter.hpp
               ../src/util/Util.cpp
               ../src/util/Util.hpp
               common/SineWaveGenerator.cpp
               common/SineWaveGenerator.hpp
               AudioDeviceGraphWithPDCTest/main.cpp)

add_executable(AudioProcessDataBufferContainerTest
               ../src/audio/engine/AudioProcessDataBufferContainer.hpp
               ../src/audio/util/AudioBufferPool.cpp
               ../src/audio/util/AudioBufferPool.hpp
               ../src/audio/util/AudioProcessDataPointerContainer.hpp
               ../src/util/IntrusivePointer.hpp
               ../src/util/IntrusiveRefCounter.cpp
               ../src/util/IntrusiveRefCounter.hpp
               AudioProcessDataBufferContainerTest/main.cpp)

add_executable(BadMemoryAccessHandlerTest
               ../src/native/Library.hpp
               ../src/native/win/Library.cpp
               ../src/native/Native.hpp
               ../src/native/linux/Native.cpp
               ../src/native/win/Native.cpp
               BadMemoryAccessHandlerTest/main.cpp)

add_executable(CPUFunctionsTest
               ../src/native/CPU.hpp
               ../src/native/win/CPU.cpp
               CPUFunctionsTest/main.cpp)

add_executable(IntrusivePointerTest
               ../src/util/IntrusivePointer.hpp
               ../src/util/IntrusiveRefCounter.cpp
               ../src/util/IntrusiveRefCounter.hpp
               IntrusivePointerTest/main.cpp)

add_executable(LockFreeQueueTest
               ../src/util/CompilerSpecifics.hpp
               ../src/util/LockFreeQueue.hpp
               ../src/util/FixedSizeLockFreeQueue.hpp
               LockFreeQueueTest/main.cpp)

add_executable(CircularDequeTest
               ../src/util/CircularDeque.hpp
               ../src/util/FixedSizeCircularDeque.hpp
               CircularDequeTest/main.cpp)

add_executable(PassDataToRealtimeThreadTest
               ../src/concurrent/PassDataToRealtimeThread.hpp
               PassDataToRealtimeThreadTest/main.cpp)

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

add_executable(ThreadAffinityTest
               ../src/native/Thread.hpp
               ../src/native/linux/Thread.cpp
               ../src/native/win/Thread.cpp
               ThreadAffinityTest/main.cpp
)

add_executable(ZeckendorfLogarithmTest ZeckendorfLogarithmTest/main.cpp)

target_link_libraries(AudioDeviceGraphTest PRIVATE ade Qt6::Core)

target_link_libraries(AudioDeviceGraphWithPDCTest PRIVATE ade Qt6::Core)

target_link_libraries(ADEUtilsTest PRIVATE ade)

target_link_libraries(AudioProcessDataBufferContainerTest PRIVATE Qt6::Core)

if(WIN32)
    target_link_libraries(BadMemoryAccessHandlerTest Qt6::Core user32 version)
elseif(LINUX)
    target_link_libraries(BadMemoryAccessHandlerTest Qt6::Core)
endif()

target_link_libraries(QStringConversionTest PRIVATE Qt6::Core)

target_link_libraries(SampleDelayTest PRIVATE Qt6::Core)

add_subdirectory(FileTreeModelTest)

add_subdirectory(SortFilterTest)