@echo off
set QTDIR=C:\dev\inst\qt\qt-4.8.7-x86-msvc2017
set PATH=%PATH%;%QTDIR%\bin
set INNO_DIR=C:\dev\inst\InnoSetup6

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"

cmake -S "cmake_qt4" -B "./_build_cmake" -G "Visual Studio 17 2022" -A Win32
cmake --build _build_cmake --config Release --target Ipponboard
cmake --build _build_cmake --target make-setup
pause
