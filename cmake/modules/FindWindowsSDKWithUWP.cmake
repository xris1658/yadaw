function(FindWindowsSDKWithUWP)
    set(WINDOWS_SDK_REGISTRY_BASE_PATH "HKLM/SOFTWARE/Microsoft/Windows Kits/Installed Roots")
    cmake_host_system_information(
        RESULT KIT_ROOT_DIR
        QUERY WINDOWS_REGISTRY
        ${WINDOWS_SDK_REGISTRY_BASE_PATH}
        VALUE "KitsRoot10"
        VIEW 32
        ERROR_VARIABLE FIND_WINDOWS_SDK_ERROR
    )
    if(FIND_WINDOWS_SDK_ERROR)
        message(FATAL_ERROR "Could not find Windows SDK: ${FIND_WINDOWS_SDK_ERROR}")
        return()
    endif()
    cmake_host_system_information(
        RESULT KIT_VERSIONS
        QUERY WINDOWS_REGISTRY
        ${WINDOWS_SDK_REGISTRY_BASE_PATH}
        SUBKEYS
        VIEW 32
        ERROR_VARIABLE FIND_WINDOWS_SDK_ERROR
    )
    if(FIND_WINDOWS_SDK_ERROR)
        message(FATAL_ERROR "Could not find Windows SDK: ${FIND_WINDOWS_SDK_ERROR}")
        return()
    endif()
    foreach(WINDOWS_SDK_VERSION IN LISTS KIT_VERSIONS)
        cmake_host_system_information(
            RESULT KIT_HAS_UWP
            QUERY WINDOWS_REGISTRY
            "${WINDOWS_SDK_REGISTRY_BASE_PATH}/${WINDOWS_SDK_VERSION}/Installed Options"
            VALUE "OptionId.UWPCPP"
            VIEW 32
            ERROR_VARIABLE FIND_WINDOWS_SDK_ERROR
        )
        if(KIT_HAS_UWP EQUAL 1)
            message(STATUS "Found UWP C++ SDK in Windows SDK version ${WINDOWS_SDK_VERSION}")
            set(WINDOWS_SDK_INCLUDE_BASE_PATH "${KIT_ROOT_DIR}Include\\${WINDOWS_SDK_VERSION}")
        elseif(FIND_WINDOWS_SDK_ERROR)
            message(STATUS "An error occurred finding UWP C++ SDK in Windows SDK version ${WINDOWS_SDK_VERSION}: ${FIND_WINDOWS_SDK_ERROR}")
        endif()
    endforeach()
    if(WINDOWS_SDK_INCLUDE_BASE_PATH)
        message(STATUS "Windows SDK include path is ${WINDOWS_SDK_INCLUDE_BASE_PATH}")
        include_directories(
            ${WINDOWS_SDK_INCLUDE_BASE_PATH}/winrt
            ${WINDOWS_SDK_INCLUDE_BASE_PATH}/cppwinrt
        )
    else()
        message(FATAL_ERROR "Could not find a Windows SDK with UWP.")
    endif()
endfunction()