cmake_minimum_required(VERSION 3.21.1)

project(YADAW VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Prerequisites and dependencies
find_package(QT NAMES Qt6 COMPONENTS Core Gui Qml Quick QuickTemplates2 LinguistTools REQUIRED)
find_package(Qt6 6.3 COMPONENTS Core Gui Qml Quick QuickTemplates2 LinguistTools REQUIRED)

find_package(ade CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_path(SQLITE_MODERN_CPP_INCLUDE_DIRS "sqlite_modern_cpp.h")
find_package(yaml-cpp
                  CONFIG REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/content
                    ${PROJECT_SOURCE_DIR}/src
                    ${CLAP_SOURCE_DIR}/include # CLAP
                    ${CLAP_HELPERS_DIR}/include # CLAP Helpers
                    ${SQLITE_MODERN_CPP_INCLUDE_DIRS}) # sqlite-modern-cpp

## VST3
set(SMTG_ADD_VSTGUI OFF)
set(SMTG_ADD_VST3_PLUGINS_SAMPLES OFF)

add_subdirectory(${VST3SDK_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)

add_subdirectory(content)

add_definitions(-DUNICODE -D_UNICODE)

add_compile_options(/W4 /Zc:wchar_t /Zc:__cplusplus)

set(YADAW_COMMON_SOURCES
    src/audio/backend/AudioGraphBackend.hpp
    src/audio/base/Automation.cpp
    src/audio/base/Automation.hpp
    src/audio/base/Channel.hpp
    src/audio/base/Gain.hpp
    src/audio/base/StereoPanning.hpp
    src/audio/base/TempoAutomation.hpp
    src/audio/device/IChannelGroup.cpp
    src/audio/device/IChannelGroup.hpp
    src/audio/device/IChannelGroupCollection.hpp
    src/audio/device/IDevice.hpp
    src/audio/host/CLAPEventList.cpp
    src/audio/host/CLAPEventList.hpp
    src/audio/host/CLAPHost.cpp
    src/audio/host/CLAPHost.hpp
    src/audio/host/VST3AttributeList.cpp
    src/audio/host/VST3AttributeList.hpp
    src/audio/host/VST3ComponentHandler.cpp
    src/audio/host/VST3ComponentHandler.hpp
    src/audio/host/VST3Host.cpp
    src/audio/host/VST3Host.hpp
    src/audio/host/VST3Message.cpp
    src/audio/host/VST3Message.hpp
    src/audio/host/VST3ParameterChanges.cpp
    src/audio/host/VST3ParameterChanges.hpp
    src/audio/host/VST3ParameterValueQueue.cpp
    src/audio/host/VST3ParameterValueQueue.hpp
    src/audio/plugin/CLAPChannelGroup.cpp
    src/audio/plugin/CLAPChannelGroup.hpp
    src/audio/plugin/CLAPPlugin.cpp
    src/audio/plugin/CLAPPlugin.hpp
    src/audio/plugin/CLAPPluginGUI.cpp
    src/audio/plugin/CLAPPluginGUI.hpp
    src/audio/plugin/CLAPPluginParameter.cpp
    src/audio/plugin/CLAPPluginParameter.hpp
    src/audio/plugin/IParameter.hpp
    src/audio/plugin/IPlugin.hpp
    src/audio/plugin/IPluginGUI.hpp
    src/audio/plugin/IPluginParameter.hpp
    src/audio/plugin/VST3ChannelGroup.cpp
    src/audio/plugin/VST3ChannelGroup.hpp
    src/audio/plugin/VST3MemoryStream.cpp
    src/audio/plugin/VST3PlugFrame.cpp
    src/audio/plugin/VST3PlugFrame.hpp
    src/audio/plugin/VST3Plugin.cpp
    src/audio/plugin/VST3Plugin.hpp
    src/audio/plugin/VST3PluginGUI.cpp
    src/audio/plugin/VST3PluginGUI.hpp
    src/audio/plugin/VST3PluginParameter.cpp
    src/audio/plugin/VST3PluginParameter.hpp
    src/audio/util/CLAPUtil.hpp
    src/audio/util/VST3Util.cpp
    src/audio/util/VST3Util.hpp
    src/base/Constants.hpp
    src/controller/AppController.cpp
    src/controller/AppController.hpp
    src/controller/AssetDirectoryController.cpp
    src/controller/AssetDirectoryController.hpp
    src/controller/AudioBackendController.cpp
    src/controller/AudioBackendController.hpp
    src/controller/AudioEngineController.hpp
    src/controller/ConfigController.cpp
    src/controller/ConfigController.hpp
    src/controller/DatabaseController.cpp
    src/controller/DatabaseController.hpp
    src/controller/GeneralSettingsController.cpp
    src/controller/GeneralSettingsController.hpp
    src/controller/LocalizationController.cpp
    src/controller/LocalizationController.hpp
    src/controller/PluginController.cpp
    src/controller/PluginController.hpp
    src/controller/PluginDirectoryController.cpp
    src/controller/PluginDirectoryController.hpp
    src/controller/PluginListController.cpp
    src/controller/PluginListController.hpp
    src/dao/AssetDirectoryTable.cpp
    src/dao/AssetDirectoryTable.hpp
    src/dao/Config.cpp
    src/dao/Config.hpp
    src/dao/DAOBase.cpp
    src/dao/DAOBase.hpp
    src/dao/Database.cpp
    src/dao/Database.hpp
    src/dao/Logging.cpp
    src/dao/Logging.hpp
    src/dao/PluginCategoryTable.cpp
    src/dao/PluginCategoryTable.hpp
    src/dao/PluginDirectoryTable.cpp
    src/dao/PluginDirectoryTable.hpp
    src/dao/PluginTable.cpp
    src/dao/PluginTable.hpp
    src/event/EventBase.cpp
    src/event/EventBase.hpp
    src/event/EventHandler.cpp
    src/event/EventHandler.hpp
    src/event/SplashScreenWorkerThread.cpp
    src/event/SplashScreenWorkerThread.hpp
    src/l10n/L10N.cpp
    src/l10n/L10N.hpp
    src/midi/MIDIInputDevice.hpp
    src/midi/MIDIOutputDevice.hpp
    src/midi/Message.hpp
    src/midi/MessageToCLAPEvent.cpp
    src/midi/MessageToCLAPEvent.hpp
    src/midi/MessageToVST3Event.cpp
    src/midi/MessageToVST3Event.hpp
    src/model/AssetDirectoryListModel.cpp
    src/model/AssetDirectoryListModel.hpp
    src/model/AudioGraphInputDeviceListModel.cpp
    src/model/AudioGraphInputDeviceListModel.hpp
    src/model/AudioGraphOutputDeviceListModel.cpp
    src/model/AudioGraphOutputDeviceListModel.hpp
    src/model/LocalizationListModel.cpp
    src/model/LocalizationListModel.hpp
    src/model/ModelInitializer.cpp
    src/model/ModelInitializer.hpp
    src/model/PluginDirectoryListModel.cpp
    src/model/PluginDirectoryListModel.hpp
    src/model/PluginListModel.cpp
    src/model/PluginListModel.hpp
    src/native/AudioGraphBackend.cpp
    src/native/AudioGraphBackendImpl.cpp
    src/native/AudioGraphBackendImpl.hpp
    src/native/CLAPNative.cpp
    src/native/CLAPNative.hpp
    src/native/Library.cpp
    src/native/Library.hpp
    src/native/LibraryImpl.cpp
    src/native/LibraryImpl.hpp
    src/native/MIDIInputDevice.cpp
    src/native/MIDIInputDeviceImpl.cpp
    src/native/MIDIInputDeviceImpl.hpp
    src/native/MIDIOutputDevice.cpp
    src/native/MIDIOutputDeviceImpl.cpp
    src/native/MIDIOutputDeviceImpl.hpp
    src/native/Native.cpp
    src/native/Native.hpp
    src/native/VST3Native.cpp
    src/native/VST3Native.hpp
    src/native/WindowsDarkModeSupport.cpp
    src/native/WindowsDarkModeSupport.hpp
    src/native/winrt/Forward.hpp
    src/native/winrt/QStringFromHString.cpp
    src/native/winrt/QStringFromHString.hpp
    src/ui/MessageDialog.cpp
    src/ui/MessageDialog.hpp
    src/ui/UI.cpp
    src/ui/UI.hpp
    src/util/Base.hpp
    src/util/CircularDeque.hpp
    src/util/CompilerSpecifics.hpp
    src/util/Constants.hpp
    src/util/Endian.cpp
    src/util/Endian.hpp
    src/util/FileHash.cpp
    src/util/FileHash.hpp
    src/util/FixedSizeCircularDeque.hpp
    src/util/FixedSizeLockFreeQueue.hpp
    src/util/FixedSizeMemoryBlock.hpp
    src/util/Integration.cpp
    src/util/Integration.hpp
    src/util/Literal.cpp
    src/util/Literal.hpp
    src/util/LockFreeQueue.hpp
    src/util/QStringUtil.cpp
    src/util/QStringUtil.hpp
    src/util/QmlUtil.hpp
    src/util/QuadraticFunction.cpp
    src/util/QuadraticFunction.hpp
    src/util/SineFunction.cpp
    src/util/SineFunction.hpp
    src/util/Stopwatch.hpp
    )

qt_add_executable(YADAW
                  rc/YADAW.rc
                  src/main.cpp
                  ${YADAW_COMMON_SOURCES}
                  )

add_executable(CLAPTest ${YADAW_COMMON_SOURCES}
               test/common/PluginWindowThread.hpp
               test/common/PluginWindowThread.cpp
               test/CLAPTest/main.cpp)

add_executable(PluginDirectoryScanTest ${YADAW_COMMON_SOURCES}
    test/PluginDirectoryScanTest/main.cpp)

add_executable(PluginScanTest ${YADAW_COMMON_SOURCES}
    test/PluginScanTest/main.cpp)

add_executable(VST3Test ${YADAW_COMMON_SOURCES}
               test/common/PluginWindowThread.hpp
               test/common/PluginWindowThread.cpp
    test/VST3Test/main.cpp)

target_precompile_headers(YADAW PRIVATE src/pch.hpp)

qt_add_qml_module(YADAW
                  URI "Main"
                  VERSION 1.0
                  NO_PLUGIN
                  QML_FILES Events.qml YADAW.qml)

if(CMAKE_BUILD_TYPE EQUAL Release OR ${CMAKE_BUILD_TYPE} EQUAL MinSizeRel)
    set_target_properties(YADAW PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

target_link_libraries(YADAW PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickTemplates2
                      contentplugin
                      ade
                      version ole32 dwmapi
                      windowsapp
                      base pluginterfaces sdk_common sdk_hosting
                      unofficial::sqlite3::sqlite3
                      yaml-cpp)

target_link_libraries(CLAPTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickTemplates2
                      contentplugin
                      version ole32 dwmapi
                      windowsapp
                      base pluginterfaces sdk_common sdk_hosting
                      unofficial::sqlite3::sqlite3
                      yaml-cpp)

target_link_libraries(PluginDirectoryScanTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickTemplates2
                      contentplugin
                      version ole32 dwmapi
                      windowsapp
                      base pluginterfaces sdk_common sdk_hosting
                      unofficial::sqlite3::sqlite3
                      yaml-cpp)

target_link_libraries(PluginScanTest PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickTemplates2
                      contentplugin
                      version ole32 dwmapi
                      windowsapp
                      base pluginterfaces sdk_common sdk_hosting
                      unofficial::sqlite3::sqlite3
                      yaml-cpp)

target_link_libraries(VST3Test PRIVATE
                      Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickTemplates2
                      contentplugin
                      version ole32 dwmapi
                      windowsapp
                      base pluginterfaces sdk_common sdk_hosting
                      unofficial::sqlite3::sqlite3
                      yaml-cpp)

message(NOTICE "Generate translation files using Qt Linguist BEFORE loading the CMake project")

file(COPY ${CMAKE_CURRENT_BINARY_DIR}/content/YADAW_zh_CN.qm
     ./l10n/osct/YADAW_zh_CN.yml
     ./l10n/osct/wqy-microhei.ttc
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/l10n/osct)

add_subdirectory(test)