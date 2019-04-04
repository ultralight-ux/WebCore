@echo off
if "%1"=="clean" GOTO CLEAN
echo Creating Ultralight - Visual Studio Projects
if not exist BuildVS mkdir BuildVS
cd BuildVS
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
set CC=cl.exe
set CXX=cl.exe
cmake .. -G "Visual Studio 14 2015 Win64"
cd ..
GOTO DONE
:CLEAN
del /f /s /q BuildVS >nul 2>&1
rmdir /s /q BuildVS >nul 2>&1
echo Working directory clean.
GOTO DONE
:DONE