@echo off
SETLOCAL
if "%1"=="release" GOTO VALID
if "%1"=="debug" GOTO VALID
if "%1"=="full_debug" GOTO VALID
GOTO SYNTAX
:VALID

CALL :NORMALIZEPATH "."
SET ROOT_DIR=%RETVAL%

CALL :NORMALIZEPATH "./src"
SET SRC_DIR=%RETVAL%

set "DIRNAME=build_cross_linux_%1"

if "%1"=="release" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=Release"
)
if "%1"=="debug" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo"
)
if "%1"=="full_debug" (
  set "FLAGS=-G "Ninja" -DCMAKE_BUILD_TYPE=Debug"
)

echo Using Clang-based Linux x86_64 Toolchain
echo NOTE: This build script does not do any remote checkout of dependencies--
echo       you must copy all dependencies to the deps folder manually.
set TOOLCHAIN_PATH="%ROOT_DIR%\Source\cmake\CrossLinuxX64Toolchain.cmake"
set "FLAGS=%FLAGS% -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_PATH% -DUSE_LOCAL_DEPS=1"

if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

cmake .. %FLAGS%
if "%1"=="vs" GOTO FINISH
ninja
GOTO FINISH
:SYNTAX
echo.
echo usage: make-cross-linux [ release ^| debug ^| full_debug ]  
echo.
echo Build type parameter descriptions:
echo.
echo     release     Build optimized release.
echo     debug       Build optimized release with debug symbols.
echo     full_debug  Built non-optimized release with debug symbols.
echo.
:FINISH
cd ..

:: ========== FUNCTIONS ==========
EXIT /B

:NORMALIZEPATH
  SET RETVAL=%~f1
  EXIT /B