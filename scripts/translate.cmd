@echo off
call %~dp0..\env_cfg.bat

"%QTDIR%\bin\lupdate" base core widgets -locations relative -no-recursive -ts i18n\de.ts -ts i18n\nl.ts

pause

"%QTDIR%\bin\linguist" i18n\de.ts i18n\nl.ts
"%QTDIR%\bin\lrelease" -compress i18n\de.ts -qm i18n\de.qm
"%QTDIR%\bin\lrelease" -compress i18n\nl.ts -qm i18n\nl.qm

pause