@echo off
SETLOCAL
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
if "%1"=="clean" GOTO CLEAN
if "%1"=="build" GOTO BUILD
if not exist BuildReleaseMin mkdir BuildReleaseMin
cd BuildReleaseMin
set CC=cl.exe
set CXX=cl.exe
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel
cd ..
GOTO DONE
:CLEAN
del /f /s /q BuildReleaseMin >nul 2>&1
rmdir /s /q BuildReleaseMin >nul 2>&1
echo Working directory clean.
GOTO DONE
:BUILD
echo(
echo  ^|                                                            ^|
echo  ^|  Mode: Release (Minimum Size)                              ^|
echo  ^|  Building via CMake/Ninja....                              ^|
echo  ^|                                                            ^|
echo(
cd BuildReleaseMin
ninja
cd ..
GOTO DONE
:DONE