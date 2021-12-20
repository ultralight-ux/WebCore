if (DEFINED TOOLCHAIN_LINUX_UBUNTU_18_04_CLANG_CMAKE)
  return ()
else ()
  set(TOOLCHAIN_LINUX_UBUNTU_18_04_CLANG_CMAKE 1)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE CACHE BOOL ON)
set(PORT "UltralightLinux")

set(UL_FFMPEG_ARGS --target-os=linux --enable-pic --disable-asm --cpu=haswell)