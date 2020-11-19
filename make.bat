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

if "%2"=="x64_uwp" (
  echo Using UWP Platform.
  set "FLAGS=%FLAGS% -DUWP_PLATFORM=1 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0"
) else (
  set "FLAGS=%FLAGS% -DUWP_PLATFORM=0 -DWINDOWS_DESKTOP_PLATFORM=1"
)

set STATIC_BUILD=0
set STATIC_CRT=0
set DEBUG_CRT=0
set USE_LOCAL_DEPS=0
set NO_JIT=0

:PROCESS_ARG
if not "%3" == "" (
  if "%3"=="static" ( 
    set STATIC_BUILD=1
  )
  if "%3"=="static_crt" ( 
    set STATIC_CRT=1
  )
  if "%3"=="debug_crt" ( 
    set DEBUG_CRT=1
  )
  if "%3"=="local" (
    set USE_LOCAL_DEPS=1
  )
  if "%3"=="no_jit" (
    set NO_JIT=1
  )
  shift
  goto PROCESS_ARG
)

if %STATIC_BUILD%==1 (
  echo Building static library.
  set "DIRNAME=%DIRNAME%_static"
) else (
  set "DIRNAME=%DIRNAME%_dll"
)
set "FLAGS=%FLAGS% -DSTATIC_BUILD=%STATIC_BUILD%"

if %STATIC_CRT%==1 (
  echo Building with static CRT
  set "DIRNAME=%DIRNAME%_MT"
) else (
  set "DIRNAME=%DIRNAME%_MD"
)
set "FLAGS=%FLAGS% -DSTATIC_CRT=%STATIC_CRT%"

if %DEBUG_CRT%==1 (
  echo Building with debug CRT
  set "DIRNAME=%DIRNAME%d"
)
set "FLAGS=%FLAGS% -DDEBUG_CRT=%DEBUG_CRT%"

if %USE_LOCAL_DEPS%==1 (
  echo Using local dependencies.
) else (
  echo Fetching dependencies from remote server.
)
set "FLAGS=%FLAGS% -DUSE_LOCAL_DEPS=%USE_LOCAL_DEPS%"

if %NO_JIT%==1 (
  echo Disabling JavaScriptCore JIT.
  set "DIRNAME=%DIRNAME%_no_jit"
) else (
  echo Enabling JavaScriptCore JIT.
)
set "FLAGS=%FLAGS% -DNO_JIT=%NO_JIT%"

call "%VCVARS%" %CFG%
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
set CC=cl.exe
set CXX=cl.exe

cmake .. %FLAGS%
if "%1"=="vs" GOTO FINISH
ninja
GOTO FINISH
:SYNTAX
echo.
echo usage: make [ release ^| full_release ^| debug ^| full_debug ^| vs ]  [ x64 ^| x64_uwp ] [ local ] [ static ] [ static_crt ]
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
echo Additional, optional build options:
echo.
echo     local       Use local dependencies in deps folder (don't fetch from server).
echo     static      Whether or not to build a static library. Disabled by default (creates a DLL).
echo     static_crt  Whether or not to use static runtime library (MT). Disabled by default (MD instead).
echo     debug_crt   Whether or not to use the debug runtime library (MDd or MTd, based on static_crt). Disabled by default.
echo     no_jit      Whether or not to build without JavaScriptCore JIT (creates smaller build). Disabled by default.
:FINISH
cd ..