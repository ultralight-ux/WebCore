@echo off
SETLOCAL
set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
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
  set "FLAGS=-G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release"
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

set "FLAGS=%FLAGS% -DPORT=UltralightWin"
set PROFILE_PERF=0
set PROFILE_MEM=0
set MEM_STATS=0
set NO_JIT=0
set LOCAL=0
set ENABLE_VIDEO=0
set NO_MIMALLOC=0
set MIMALLOC_OVERRIDEN=0

:PROCESS_ARG
if not "%3" == "" (
  if "%3"=="profile_perf" ( 
    set PROFILE_PERF=1
  )
  if "%3"=="profile_mem" ( 
    set PROFILE_MEM=1
  )
  if "%3"=="mem_stats" ( 
    set MEM_STATS=1
  )
  if "%3"=="local" (
    set LOCAL=1
  )
  if "%3"=="no_jit" (
    set NO_JIT=1
  )
  if "%3"=="no_mimalloc" (
    set NO_MIMALLOC=1
  )
  if "%3"=="enable_video" (
    set ENABLE_VIDEO=1
  )
  shift
  goto PROCESS_ARG
)

if %PROFILE_PERF%==1 (
  echo Enabling Tracy Performance Profiling
  set "FLAGS=%FLAGS% -DUL_PROFILE_PERFORMANCE=1"
  set "DIRNAME=%DIRNAME%_pp"
)

if %PROFILE_MEM%==1 (
  echo Enabling Tracy Memory Profiling
  set "FLAGS=%FLAGS% -DUL_PROFILE_MEMORY=1 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"
  set "DIRNAME=%DIRNAME%_pm"
  set NO_MIMALLOC=1
  set MIMALLOC_OVERRIDEN=1
)

if %MEM_STATS%==1 (
  echo Enabling Memory Statistics
  set "FLAGS=%FLAGS% -DUL_ENABLE_MEMORY_STATS=1 -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"
  set "DIRNAME=%DIRNAME%_ms"
  set NO_MIMALLOC=1
  set MIMALLOC_OVERRIDEN=1
)

if %LOCAL%==1 (
  echo Using local deps.
  set "DIRNAME=%DIRNAME%_local"
  set "FLAGS=%FLAGS% -DUSE_LOCAL_DEPS=1"
) else (
  echo Fetching deps from remote server.
  set "FLAGS=%FLAGS% -DUSE_LOCAL_DEPS=0"
)

if %NO_JIT%==1 (
  echo Disabling JavaScriptCore JIT.
  set "DIRNAME=%DIRNAME%_nj"
  set "FLAGS=%FLAGS% -DUL_ENABLE_JIT=0"
) else (
  echo Enabling JavaScriptCore JIT.
  set "FLAGS=%FLAGS% -DUL_ENABLE_JIT=1"
)

if %ENABLE_VIDEO%==1 (
  echo Enabling Video/Audio.
  set "DIRNAME=%DIRNAME%_ev"
  set "FLAGS=%FLAGS% -DUL_ENABLE_VIDEO=1"
) else (
  set "FLAGS=%FLAGS% -DUL_ENABLE_VIDEO=0"
)

if %NO_MIMALLOC%==1 (
  echo Disabling mimalloc.
  if %MIMALLOC_OVERRIDEN%==0 (
    set "DIRNAME=%DIRNAME%_nm"
  )
  set "FLAGS=%FLAGS% -DUL_ENABLE_MIMALLOC=0"
) else (
  echo Enabling mimalloc.
  set "FLAGS=%FLAGS% -DUL_ENABLE_MIMALLOC=1"
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
echo usage: make [ release ^| release_min ^| release_dbg ^| debug ^| vs ]  [ x64 ] [ local ] [ no_jit ] [ no_mimalloc ] [ enable_video ] [ profile_perf ^| profile_mem ^| mem_stats ]
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
echo     local          Use local dependencies from 'deps' folder instead of pulling from remote server.
echo     no_jit         Optionally build without JavaScriptCore JIT (creates smaller build).
echo     no_mimalloc    Optionally build without mimalloc allocator (uses OS allocator).
echo     enable_video   Optionally build with Video/Audio support (requires msys2 and meson packages).
echo     profile_perf   Whether or not to enable runtime performance profiling.
echo     profile_mem    Whether or not to enable runtime memory profiling.
echo     mem_stats      Whether or not to enable runtime memory statistics.
:FINISH
cd ..