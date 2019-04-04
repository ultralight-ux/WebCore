@echo off
SETLOCAL
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64_x86
if "%1"=="clean" GOTO CLEAN
if "%1"=="build" GOTO BUILD
if not exist BuildReleaseMin32 mkdir BuildReleaseMin32
cd BuildReleaseMin32
set CC=cl.exe
set CXX=cl.exe
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel
cd ..
GOTO DONE
:CLEAN
del /f /s /q BuildReleaseMin32 >nul 2>&1
rmdir /s /q BuildReleaseMin32 >nul 2>&1
echo Working directory clean.
GOTO DONE
:BUILD
echo(
echo  ^|                                                            ^|
echo  ^|  Mode: Release 32-bit (Minimum Size)                       ^|
echo  ^|  Building via CMake/Ninja....                              ^|
echo  ^|                                                            ^|
echo(
cd BuildReleaseMin32
ninja
cd ..
GOTO DONE
:DONE