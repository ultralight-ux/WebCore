add_definitions(-DNOMINMAX -DUNICODE -D_UNICODE -D_WINDOWS -DWINVER=0x601 -DWTF_PLATFORM_ULTRALIGHT=1)

WEBKIT_OPTION_BEGIN()
include(UltralightOptions)

# TODO: Re-enable FTL JIT on Windows, need to re-apply patches and test against latest
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_FTL_JIT PUBLIC OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEBASSEMBLY PRIVATE OFF)

WEBKIT_OPTION_END()

set(WebCore_LIBRARY_TYPE SHARED)

include(UltralightDefinitions)

set(ICU_LIBRARIES icuuc icuin icudt)

if (MSVC)
    add_definitions(
        /wd4018 /wd4068 /wd4099 /wd4100 /wd4127 /wd4138 /wd4146 /wd4180 /wd4189
        /wd4201 /wd4206 /wd4244 /wd4251 /wd4267 /wd4275 /wd4288 /wd4291 /wd4305
        /wd4309 /wd4344 /wd4355 /wd4389 /wd4396 /wd4456 /wd4457 /wd4458 /wd4459
        /wd4481 /wd4503 /wd4505 /wd4510 /wd4512 /wd4530 /wd4610 /wd4611 /wd4646
        /wd4702 /wd4706 /wd4722 /wd4800 /wd4819 /wd4951 /wd4952 /wd4996 /wd6011
        /wd6031 /wd6211 /wd6246 /wd6255 /wd6387 /wd4577
    )

    if (UL_ENABLE_STATIC_BUILD)
        add_definitions(-DULTRALIGHT_STATIC_BUILD -DJS_NO_EXPORT)
    endif ()

    # We do not use exceptions
    add_definitions(-D_HAS_EXCEPTIONS=0)
    add_compile_options(/EHa- /EHc- /EHs- /fp:except-)

    # We have some very large object files that have to be linked
    add_compile_options(/analyze- /bigobj)

    # Use CRT security features
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1)
    
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE=1)
    
    # If <winsock2.h> is not included before <windows.h> redefinition errors occur
    # unless _WINSOCKAPI_ is defined before <windows.h> is included
    add_definitions(-D_WINSOCKAPI_=)
    
    IF( NOT CMAKE_BUILD_TYPE )
       SET( CMAKE_BUILD_TYPE Release )
    ENDIF()
    
    # Needed for bmalloc on Windows
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    
    if (${CMAKE_BUILD_TYPE} MATCHES Debug)
        add_compile_options(/GS)

        if (NOT UL_ENABLE_DEBUG_CHECKS)
            add_compile_options(/DNDEBUG)
        endif ()
        
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:NOREF /OPT:NOICF")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /OPT:NOREF /OPT:NOICF")
    elseif (${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
        # Create pdb files for debugging purposes
        add_compile_options(/Zi /GS)

        if (NOT UL_ENABLE_DEBUG_CHECKS)
            add_compile_options(/DNDEBUG)
        endif ()
        
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:NOREF /OPT:NOICF /DEBUG:FASTLINK")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /OPT:NOREF /OPT:NOICF /DEBUG:FASTLINK")
    elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
        add_compile_options(/O2 /Gy /Gw /cgthreads8)
    else ()
        add_compile_options(/O1 /Os /Gy /Gw /cgthreads8)
        
        # Uncomment this block to add PDB to optimized release builds
        # add_compile_options(/Zi)
        # set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /OPT:REF /OPT:ICF /DEBUG")
        # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /OPT:REF /OPT:ICF /DEBUG")
        # set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /OPT:REF /OPT:ICF /DEBUG")
        # set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /OPT:REF /OPT:ICF /DEBUG")
    endif ()

    if (NOT ${CMAKE_GENERATOR} MATCHES "Ninja")
        link_directories("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
        add_definitions(/MP)
    endif ()

    string(REGEX REPLACE "(/EH[a-z]+) " "\\1- " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable C++ exceptions
    string(REGEX REPLACE "/EHsc$" "/EHs- /EHc- " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Disable C++ exceptions
    string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # Warnings are important

    foreach (flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        # Reset all project configurations to use /MD so we can override below
        string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
        string(REGEX REPLACE "/MDd" "/MD" ${flag_var} "${${flag_var}}")
        string(REGEX REPLACE "/D_DEBUG" "" ${flag_var} "${${flag_var}}")

        if (CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreaded")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        elseif (CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDLL")
            # no-op, already /MD
        elseif(CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDebug")
            string(REGEX REPLACE "/MD" "/MTd" ${flag_var} "${${flag_var}}")
        elseif(CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDebugDLL")
            string(REGEX REPLACE "/MD" "/MDd" ${flag_var} "${${flag_var}}")
        else()
            message(FATAL_ERROR "Unknown MSVC runtime library: '${CMAKE_MSVC_RUNTIME_LIBRARY}'")
        endif()
    endforeach ()

    if (CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreaded")
        message(STATUS "Building projects with MultiThreaded Static Runtime (/MT)")
    elseif (CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDLL")
        message(STATUS "Building projects with MultiThreaded DLL Runtime (/MD)")
    elseif(CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDebug")
        message(STATUS "Building projects with MultiThreaded Static Debug Runtime (/MTd)")
    elseif(CMAKE_MSVC_RUNTIME_LIBRARY STREQUAL "MultiThreadedDebugDLL")
        message(STATUS "Building projects with MultiThreaded DLL Debug Runtime (/MDd)")
    else()
        message(FATAL_ERROR "Unknown MSVC runtime library: '${CMAKE_MSVC_RUNTIME_LIBRARY}'")
    endif()
endif ()

if (COMPILER_IS_CLANG_CL)
    # FIXME: Building with clang-cl seemed to fail with 128 bit int support
    set(HAVE_INT128_T OFF)
    list(REMOVE_ITEM _WEBKIT_CONFIG_FILE_VARIABLES HAVE_INT128_T)

    add_definitions(-D__CLANG_CL__=1)
endif ()
