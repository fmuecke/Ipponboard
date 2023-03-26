@echo off
call ..\env_cfg.bat
"%qtdir%\qmake" test.pro -tp vc