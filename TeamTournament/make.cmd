@echo off
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86
jom clean > nul
qmake
jom /S /L /F makefile.release
pause