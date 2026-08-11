# TSRT stands for [The smooth resize test](https://raphlinus.github.io/rust/gui/2019/06/21/smooth-resize-test.html).
option(BUILD_D3D11_TSRT_ADAPTATION "Build scene graph adaptation with tweaked D3D11 for smooth resizing" OFF)

set(Qt6_BASE_SOURCES
    src/gui/rhi/qrhid3d11_p.h
    src/gui/rhi/qrhid3d11.cpp
)
set(Qt6_DECLARATIVE_SOURCES
    src/quick/scenegraph/qsgrhisupport.cpp
    src/quick/scenegraph/qsgrhisupport_p.h
    src/quick/scenegraph/qsgcontextplugin.cpp
    src/quick/scenegraph/qsgcontextplugin_p.h
)

function(test_qt_source_dirs)
    if(NOT QT_FEATURE_thread)
        message(STATUS "`QT_FEATURE_thread` not defined, skipping `BUILD_D3D11_TSRT_ADAPTATION`")
    endif()
    if(NOT DEFINED Qt6_BASE_SOURCE_DIR OR NOT DEFINED Qt6_DECLARATIVE_SOURCE_DIR)
        message(STATUS "`Qt6_BASE_SOURCE_DIR` or `Qt6_DECLARATIVE_SOURCE_DIR` not defined, skipping `BUILD_D3D11_TSRT_ADAPTATION`")
        message(STATUS "`Qt6_BASE_SOURCE_DIR` is set to ${Qt6_BASE_SOURCE_DIR}")
        message(STATUS "`Qt6_DECLARATIVE_SOURCE_DIR` is set to ${Qt6_DECLARATIVE_SOURCE_DIR}")
        set(BUILD_D3D11_TSRT_ADAPTATION OFF)
        return()
    endif()
    if(EXISTS ${Qt6_BASE_SOURCE_DIR} AND EXISTS ${Qt6_DECLARATIVE_SOURCE_DIR})
        foreach(Qt6_BASE_SOURCE IN LISTS Qt6_BASE_SOURCES)
            if(EXISTS ${Qt6_BASE_SOURCE_DIR}/${Qt6_BASE_SOURCE})
                message(STATUS "${Qt6_BASE_SOURCE_DIR}/${Qt6_BASE_SOURCE} found")
            else()
                message(STATUS "${Qt6_BASE_SOURCE_DIR}/${Qt6_BASE_SOURCE} not found, unsetting Qt6_BASE_SOURCE")
                unset(Qt6_BASE_SOURCE_DIR)
                break()
            endif()
        endforeach()
        foreach(Qt6_DECLARATIVE_SOURCE IN LISTS Qt6_DECLARATIVE_SOURCES)
            if(EXISTS ${Qt6_DECLARATIVE_SOURCE_DIR}/${Qt6_DECLARATIVE_SOURCE})
                message(STATUS "${Qt6_DECLARATIVE_SOURCE_DIR}/${Qt6_DECLARATIVE_SOURCE} found")
            else()
                message(STATUS "${Qt6_DECLARATIVE_SOURCE_DIR}/${Qt6_DECLARATIVE_SOURCE} not found, unsetting Qt6_DECLARATIVE_SOURCE_DIR")
                unset(Qt6_DECLARATIVE_SOURCE_DIR)
            endif()
        endforeach()
        if(DEFINED Qt6_BASE_SOURCE_DIR AND DEFINED Qt6_DECLARATIVE_SOURCE_DIR)
            set(BUILD_D3D11_TSRT_ADAPTATION ON)
        else()
            set(BUILD_D3D11_TSRT_ADAPTATION OFF)
        endif()
    else()
        set(BUILD_D3D11_TSRT_ADAPTATION OFF)
    endif()
endfunction()

if(BUILD_D3D11_TSRT_ADAPTATION)
    message(STATUS "BUILD_D3D11_TSRT_ADAPTATION is now set to ON. Check if the source directories contain files that we need...")
    test_qt_source_dirs()
endif()
message(STATUS "BUILD_D3D11_TSRT_ADAPTATION is now set to ${BUILD_D3D11_TSRT_ADAPTATION}")
if(BUILD_D3D11_TSRT_ADAPTATION)
    qt_add_plugin(
        D3D11TSRTAdaptation STATIC
        CLASS_NAME D3D11TSRTAdaptationPlugin
        src/Plugin.cpp
        src/Plugin.hpp
    )
    target_link_libraries(
        D3D11TSRTAdaptation
        PUBLIC Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickPrivate)
endif()
