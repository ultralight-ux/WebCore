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
if "%2"=="x64" (
  set CFG=amd64
  GOTO VALID
)
if "%2"=="x64_uwp" (
  set CFG=amd64 uwp
  GOTO VALID
)
GOTO SYNTAX
:VALID
set "DIRNAME=build_%1_%2"
if "%1"=="vs" (
  set "FLAGS=-G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo"
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
set "EXTRA_FLAGS=-DUWP_PLATFORM=0 -DWINDOWS_DESKTOP_PLATFORM=1"
if "%2"=="x64_uwp" (
  echo Using UWP Platform.
  set "EXTRA_FLAGS=-DUWP_PLATFORM=1 -DWINDOWS_DESKTOP_PLATFORM=0 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0"
)
if "%3"=="local" (
  echo Using local deps.
  set "EXTRA_FLAGS=%EXTRA_FLAGS% -DUSE_LOCAL_DEPS=1"
)
cmake .. %FLAGS% %EXTRA_FLAGS%
if "%1"=="vs" GOTO FINISH
ninja
GOTO FINISH
:SYNTAX
echo.
echo usage: make [ release ^| full_release ^| debug ^| full_debug ^| vs ]  [ x64 ^| x64_uwp ] [ local ]
echo.
echo Build type parameter descriptions:
echo.
echo     release       Build optimized, minimum-size release via Ninja/VS2019
echo     full_release  Build optimized, maximum-speed release via Ninja/VS2019
echo     debug         Build optimized release with debug symbols via Ninja/VS2019
echo     full_debug    Build non-optimized release with debug symbols via Ninja/VS2019
echo     vs            Generate VS2019 project files, does not perform a build.
echo.
echo Configuration parameter descriptions:
echo.
echo     x64         Compile binaries for the x64 (amd64) platform.
echo     x64_uwp     Compile binaries for the x64 (amd64) platform with UWP toolchain.
echo.
echo Optional third parameter:
echo.
echo     local       Optional third parameter to use local dependencies in deps folder.
echo.
:FINISH
cd ..