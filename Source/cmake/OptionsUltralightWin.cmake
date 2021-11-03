add_definitions(-DNOMINMAX -DUNICODE -D_UNICODE -D_WINDOWS -DWINVER=0x601 -DWTF_PLATFORM_ULTRALIGHT=1)

WEBKIT_OPTION_BEGIN()
include(UltralightOptions)
WEBKIT_OPTION_END()

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

    if (UWP_PLATFORM)
        add_definitions(-DUWP_PLATFORM) 
    endif ()

    if (WINDOWS_DESKTOP_PLATFORM)
        add_definitions(-DWINDOWS_DESKTOP_PLATFORM)
    endif ()

    if (STATIC_BUILD)
        add_definitions(-DULTRALIGHT_STATIC_BUILD -DJS_NO_EXPORT)
    endif ()

    # We do not use exceptions
    add_definitions(-D_HAS_EXCEPTIONS=0)
    add_compile_options(/EHa- /EHc- /EHs- /fp:except-)

    # We have some very large object files that have to be linked
    add_compile_options(/analyze- /bigobj)

    # Use CRT security features
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1)
    
    IF( NOT CMAKE_BUILD_TYPE )
       SET( CMAKE_BUILD_TYPE Release )
    ENDIF()
    
    # Needed for bmalloc on Windows
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LARGEADDRESSAWARE")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    
    if (${CMAKE_BUILD_TYPE} MATCHES Debug)
        # Create pdb files for debugging purposes
        add_compile_options(/GS /DNDEBUG)
        
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG /OPT:NOREF /OPT:NOICF")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /OPT:NOREF /OPT:NOICF")
    elseif (${CMAKE_BUILD_TYPE} MATCHES RelWithDebInfo)
        # Create pdb files for debugging purposes
        add_compile_options(/Zi /GS /DNDEBUG)
        
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

        if (${STATIC_CRT})
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif ()

        if (${DEBUG_CRT})
            string(REGEX REPLACE "/MD" "/MDd" ${flag_var} "${${flag_var}}")
            string(REGEX REPLACE "/MT" "/MTd" ${flag_var} "${${flag_var}}")
        endif ()
    endforeach ()

    if (${STATIC_CRT})
        if (${DEBUG_CRT})
            message(STATUS "Building projects with MultiThreaded Static Debug Runtime (/MTd)")
        else ()
            message(STATUS "Building projects with MultiThreaded Static Runtime (/MT)")
        endif ()
    else ()
        if (${DEBUG_CRT})
            message(STATUS "Building projects with MultiThreaded DLL Debug Runtime (/MDd)")
        else ()
            message(STATUS "Building projects with MultiThreaded DLL Runtime (/MD)")
        endif ()
    endif ()
endif ()

if (COMPILER_IS_CLANG_CL)
    # FIXME: Building with clang-cl seemed to fail with 128 bit int support
    set(HAVE_INT128_T OFF)
    list(REMOVE_ITEM _WEBKIT_CONFIG_FILE_VARIABLES HAVE_INT128_T)

    add_definitions(-D__CLANG_CL__=1)
endif ()
