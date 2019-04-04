@echo off
SETLOCAL
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
if "%1"=="clean" GOTO CLEAN
if "%1"=="build" GOTO BUILD
if not exist BuildReleaseDbg mkdir BuildReleaseDbg
cd BuildReleaseDbg
set CC=cl.exe
set CXX=cl.exe
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cd ..
GOTO DONE
:CLEAN
del /f /s /q BuildReleaseDbg >nul 2>&1
rmdir /s /q BuildReleaseDbg >nul 2>&1
echo Working directory clean.
GOTO DONE
:BUILD
echo(
echo  ^|                                                            ^|
echo  ^|  Mode: Release (Devel w/ Debug Symbols)                    ^|
echo  ^|  Building via CMake/Ninja....                              ^|
echo  ^|                                                            ^|
echo(
cd BuildReleaseDbg
ninja
cd ..
GOTO DONE
:DONE