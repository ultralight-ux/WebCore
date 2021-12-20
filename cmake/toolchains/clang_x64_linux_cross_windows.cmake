#
# A toolchain to cross-compile Linux x86_64 bins on a Windows host.
#
# This file assumes you have installed Unreal Engine's Linux toolchain that is located here:
#   <https://docs.unrealengine.com/4.26/en-US/SharingAndReleasing/Linux/GettingStarted/>
#
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_CROSSCOMPILING TRUE)
set(UNIX TRUE)
set(MSVC FALSE)

set(PORT "UltralightLinux")

# The Clang 'target triple' for the target platform
set(triple x86_64-unknown-linux-gnu)

# The environment var 'LINUX_MULTIARCH_ROOT' is set on Windows after installing UE's Linux toolchain.
set(CMAKE_SYSROOT $ENV{LINUX_MULTIARCH_ROOT}${triple}/)

# We need to make sure all slahes are forward slashes in paths.
string(REPLACE "\\" "/" CMAKE_SYSROOT ${CMAKE_SYSROOT})

set(TOOLCHAIN_PATH ${CMAKE_SYSROOT}bin/)

set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})

set(CMAKE_C_COMPILER_ID "Clang")
set(CMAKE_CXX_COMPILER_ID "Clang")

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

# Uncomment this line if you want to skip compilation checks (useful if the toolchain doesn't support linking)
# set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        ${TOOLCHAIN_PATH}llvm-ar.exe)
set(CMAKE_ASM_COMPILER              ${TOOLCHAIN_PATH}clang.exe)
set(CMAKE_C_COMPILER                ${TOOLCHAIN_PATH}clang.exe)
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PATH}clang++.exe)
set(CMAKE_LINKER                    ${TOOLCHAIN_PATH}lld.exe)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PATH}x86_64-unknown-linux-gnu-objcopy.exe CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${TOOLCHAIN_PATH}x86_64-unknown-linux-gnu-ranlib.exe CACHE INTERNAL "")
set(CMAKE_SIZE                      ${TOOLCHAIN_PATH}x86_64-unknown-linux-gnu-size.exe CACHE INTERNAL "")
set(CMAKE_STRIP                     ${TOOLCHAIN_PATH}x86_64-unknown-linux-gnu-strip.exe CACHE INTERNAL "")

set(CMAKE_C_FLAGS                   "-march=core2 -mtune=haswell -fno-exceptions -fvisibility=hidden" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${CMAKE_C_FLAGS}" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG             "-Og -g " CACHE INTERNAL "")
set(CMAKE_C_FLAGS_MINSIZEREL        "-Os -DNDEBUG -ffunction-sections -fdata-sections" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "-Ofast -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO    "-O2 -g -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)