@echo off
SETLOCAL
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
if "%1"=="clean" GOTO CLEAN
if "%1"=="build" GOTO BUILD
if not exist Build mkdir Build
cd Build
set CC=cl.exe
set CXX=cl.exe
cmake .. -G "Ninja"
cd ..
GOTO DONE
:CLEAN
del /f /s /q Build >nul 2>&1
rmdir /s /q Build >nul 2>&1
echo Working directory clean.
GOTO DONE
:BUILD
echo(
echo(
echo  ^|                                                            ^|
echo  ^|  Mode: Debug                                               ^|
echo  ^|  Building via CMake/Ninja....                              ^|
echo  ^|                                                            ^|
echo(
cd Build
ninja
cd ..
GOTO DONE
:DONE