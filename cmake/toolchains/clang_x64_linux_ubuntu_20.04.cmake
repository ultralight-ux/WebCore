if (DEFINED TOOLCHAIN_LINUX_UBUNTU_20_04_CLANG_CMAKE)
  return ()
else ()
  set(TOOLCHAIN_LINUX_UBUNTU_20_04_CLANG_CMAKE 1)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL ON)
set(PORT "UltralightLinux" CACHE INTERNAL "" FORCE)

set(CMAKE_C_FLAGS                   "-march=nehalem -mtune=haswell -fno-exceptions -fPIC" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${CMAKE_C_FLAGS}" CACHE INTERNAL "")

set(UL_FFMPEG_ARGS --target-os=linux --enable-pic --disable-asm --cpu=haswell)