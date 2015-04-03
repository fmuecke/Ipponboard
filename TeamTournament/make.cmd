@echo off
call "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" x86
jom clean > nul
"%QTDIR%\qmake.exe"
jom /S /L /F makefile.release
pause