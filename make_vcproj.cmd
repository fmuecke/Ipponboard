@echo off
call env_cfg.bat
"%QTDIR%\qmake" -recursive -tp vc