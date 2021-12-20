if(DEFINED COMPILER_CL_CMAKE_)
  return()
else()
  set(COMPILER_CL_CMAKE_ 1)
endif()

find_program(CMAKE_C_COMPILER cl)
find_program(CMAKE_CXX_COMPILER cl)

if(NOT CMAKE_C_COMPILER)
  show_fatal_error("MSVC compiler 'cl' not found! Please run CMake from a Visual Studio Native Tools command prompt.")
endif()

if(NOT CMAKE_CXX_COMPILER)
  show_fatal_error("MSVC compiler 'cl' not found! Please run CMake from a Visual Studio Native Tools command prompt.")
endif()

set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}" CACHE STRING "C compiler" FORCE)

set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}" CACHE STRING "C++ compiler" FORCE)