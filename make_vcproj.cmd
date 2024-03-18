@echo off
call env_cfg.bat
"%QTDIR%\bin\qmake" -recursive -tp vc