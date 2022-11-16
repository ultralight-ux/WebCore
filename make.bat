@echo off
SETLOCAL
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if "%1"=="release" GOTO CHECKTYPE
if "%1"=="release_min" GOTO CHECKTYPE
if "%1"=="release_dbg" GOTO CHECKTYPE
if "%1"=="debug" GOTO CHECKTYPE
if "%1"=="vs" GOTO CHECKTYPE
GOTO SYNTAX
:CHECKTYPE
if "%2"=="x64" (
  set CFG=amd64
  GOTO VALID
)

GOTO SYNTAX
:VALID

set "DIRNAME=build_%1_%2"
set SKIP_BUILD=0

if "%1"=="vs" (
  set "FLAGS=-G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release"
  set SKIP_BUILD=1
)
if "%1"=="release" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=Release"
)
if "%1"=="release_min" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel"
)
if "%1"=="release_dbg" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo"
)
if "%1"=="debug" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=Debug"
)

set "FLAGS=%FLAGS% -DUWP_PLATFORM=0"
set PROFILE_PERF=0
set PROFILE_MEM=0
set MEM_STATS=0
set NO_JIT=0
set NO_VIDEO=0
set NO_MIMALLOC=0
set ALLOC_OVERRIDE=0
set USE_LOCAL_DEPS=0

:PROCESS_ARG
if not "%3" == "" (
  if "%3"=="profile_perf" ( 
    set PROFILE_PERF=1
  )
  if "%3"=="profile_mem" ( 
    set PROFILE_MEM=1
    set NO_MIMALLOC=1
  )
  if "%3"=="mem_stats" ( 
    set MEM_STATS=1
    set NO_MIMALLOC=1
  )
  if "%3"=="no_jit" (
    set NO_JIT=1
  )
  if "%3"=="no_video" (
    set NO_VIDEO=1
  )
  if "%3"=="no_mimalloc" (
    set NO_MIMALLOC=1
  )
  if "%3"=="alloc_override" ( 
    set ALLOC_OVERRIDE=1
    set NO_MIMALLOC=1
  )
  shift
  goto PROCESS_ARG
)

if %USE_LOCAL_DEPS%==1 (
  echo Using local dependencies.
  set "DIRNAME=%DIRNAME%_local"
) else (
  echo Fetching dependencies from remote server.
)
set "FLAGS=%FLAGS% -DUSE_LOCAL_DEPS=%USE_LOCAL_DEPS%"

if %PROFILE_PERF%==1 (
  echo Enabling Tracy Performance Profiling
  set "FLAGS=%FLAGS% -DUL_PROFILE_PERFORMANCE=1"
  set "DIRNAME=%DIRNAME%_profile_perf"
)

if %PROFILE_MEM%==1 (
  echo Enabling Tracy Memory Profiling
  set "FLAGS=%FLAGS% -DUL_PROFILE_MEMORY=1 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"
  set "DIRNAME=%DIRNAME%_profile_mem"
)

if %MEM_STATS%==1 (
  echo Enabling Memory Statistics
  set "FLAGS=%FLAGS% -DUL_ENABLE_MEMORY_STATS=1 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"
  set "DIRNAME=%DIRNAME%_mem_stats"
)

if %ALLOC_OVERRIDE%==1 (
  echo Enabling Allocator Override
  set "FLAGS=%FLAGS% -DUL_ENABLE_ALLOCATOR_OVERRIDE=1 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"
  set "DIRNAME=%DIRNAME%_ao"
)

if %NO_JIT%==1 (
  echo Disabling JavaScriptCore JIT.
  set "DIRNAME=%DIRNAME%_no_jit"
  set "FLAGS=%FLAGS% -DUL_ENABLE_JIT=0"
) else (
  echo Enabling JavaScriptCore JIT.
  set "FLAGS=%FLAGS% -DUL_ENABLE_JIT=1"
)

if %NO_VIDEO%==1 (
  echo Disabling Video/Audio.
  set "DIRNAME=%DIRNAME%_no_video"
  set "FLAGS=%FLAGS% -DUL_ENABLE_VIDEO=0"
) else (
  echo Enabling Video/Audio.
  set "FLAGS=%FLAGS% -DUL_ENABLE_VIDEO=1"
)

if %NO_MIMALLOC%==1 (
  echo Disabling mi-malloc.
  set "FLAGS=%FLAGS% -DUL_ENABLE_MIMALLOC=0"
) else (
  echo Enabling mi-malloc.
)

call "%VCVARS%" %CFG%
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
set CC=cl.exe
set CXX=cl.exe

cmake .. %FLAGS%
if %SKIP_BUILD%==1 (
  GOTO FINISH
)
ninja
ninja install
GOTO FINISH
:SYNTAX
echo.
echo usage: make [ release ^| release_min ^| release_dbg ^| debug ^| vs ]  [ x64 ] [ local ] [ no_jit ] [ no_video ] [ no_mimalloc ] [ profile_perf ^| profile_mem ^| mem_stats ^| alloc_override ]
echo.
echo Build type parameter descriptions:
echo.
echo     release        Build release optimized for performance via Ninja/VS2019
echo     release_min    Build release optimized for size via Ninja/VS2019
echo     release_dbg    Build release with debug symbols via Ninja/VS2019
echo     debug          Build debug with all checks and debug symbols via Ninja/VS2019
echo     vs             Generate VS2019 project files, does not perform a build.
echo.
echo Configuration parameter descriptions:
echo.
echo     x64            Compile binaries for the x64 (amd64) platform.
echo.
echo Additional, optional build options:
echo.
echo     local          Use local dependencies in deps folder (don't fetch from server).
echo     no_jit         Optionally build without JavaScriptCore JIT (creates smaller build).
echo     no_video       Optionally build without Video/Audio (creates smaller build).
echo     no_mimalloc    Optionally build without mi-malloc.
echo     profile_perf   Whether or not to enable runtime performance profiling.
echo     profile_mem    Whether or not to enable runtime memory profiling.
echo     mem_stats      Whether or not to enable runtime memory statistics.
echo     alloc_override  Whether or not to enable global allocator override.
:FINISH
cd ..