@echo off
SETLOCAL
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if "%1"=="release" GOTO CHECKTYPE
if "%1"=="full_release" GOTO CHECKTYPE
if "%1"=="debug" GOTO CHECKTYPE
if "%1"=="full_debug" GOTO CHECKTYPE
if "%1"=="vs" GOTO CHECKTYPE
GOTO SYNTAX
:CHECKTYPE
if "%2"=="x86" (
  set CFG=amd64_x86
  GOTO VALID
)
if "%2"=="x64" (
  set CFG=amd64
  GOTO VALID
)
)
if "%2"=="x64_uwp" (
  set CFG=amd64 uwp
  GOTO VALID
)
GOTO SYNTAX
:VALID
set "DIRNAME=build_%1_%2"
if "%1"=="vs" (
  if "%2"=="x86" (
    set "FLAGS=-G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=RelWithDebInfo"
  ) else (
    set "FLAGS=-G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo"
  )
)
if "%1"=="release" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel"
)
if "%1"=="full_release" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=Release"
)
if "%1"=="debug" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo"
)
if "%1"=="full_debug" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=Debug"
)
call "%VCVARS%" %CFG%
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
set CC=cl.exe
set CXX=cl.exe
set "EXTRA_FLAGS=-DUWP_PLATFORM=0"
if "%2"=="x64_uwp" (
  echo Using UWP Platform.
  set "EXTRA_FLAGS=-DUWP_PLATFORM=1 -DUSE_LOCAL_DEPS=1 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0"
)
cmake .. %FLAGS% %EXTRA_FLAGS%
if "%1"=="vs" GOTO FINISH
ninja
GOTO FINISH
:SYNTAX
echo.
echo usage: make [ release ^| debug ^| vs ]  [ x64 ^| x64_uwp ^| x86 ]
echo.
echo Build type parameter descriptions:
echo.
echo     release     Build minimum-size release via Ninja/VS2019
echo     debug       Build release with debug symbols via Ninja/VS2019
echo     vs          Generate VS2019 project files, does not perform a build.
echo.
echo Configuration parameter descriptions:
echo.
echo     x64         Compile binaries for the x64 (amd64) platform.
echo     x86         Cross-compile binaries for the x86 (amd64_x86) platform.
echo.
:FINISH
cd ..