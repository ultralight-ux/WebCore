if (NOT CMAKE_TOOLCHAIN_FILE)
    if (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
        get_filename_component(CMAKE_TOOLCHAIN_FILE "../toolchains/msvc_x64_windows.cmake"
                               REALPATH BASE_DIR "${CMAKE_CURRENT_LIST_DIR}") 
    elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
        get_filename_component(CMAKE_TOOLCHAIN_FILE "../toolchains/clang_arm64_macos_11.0.cmake"
                               REALPATH BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")    
    elseif (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
        get_filename_component(CMAKE_TOOLCHAIN_FILE "../toolchains/clang_x64_linux_ubuntu_18.04.cmake"
                               REALPATH BASE_DIR "${CMAKE_CURRENT_LIST_DIR}")    
    else ()
        message(FATAL_ERROR "Unable to detect target platform. Please manually specify a CMake toolchain file "
                            "or define your platform in toolchains/autodetect-toolchain.cmake")
    endif ()
endif ()

message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")