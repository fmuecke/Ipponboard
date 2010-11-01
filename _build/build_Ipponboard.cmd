@echo off
REM -- init
echo -- setting environment variables
call Scripts\Ipponbaord_set_vars.cmd

REM -- clean up
echo -- cleaning up build dir
cd "%BASE_DIR%"
call _build\Scripts\clear_build_dir.cmd

REM
REM -- build
REM 
echo -- building main program files
cd "%BASE_DIR%\base"
REM -- updating version info...
call create_versioninfo.cmd
cd "%BASE_DIR%"
call _build\Scripts\Ipponboard_clean.cmd
cd "%BASE_DIR%"
call _build\Scripts\Ipponboard_compile.cmd

echo -- building GamepadDemo application
cd "%BASE_DIR%"
call _build\Scripts\GamepadDemo_clean.cmd
cd "%BASE_DIR%"
call _build\Scripts\GamepadDemo_compile.cmd

REM -- create doc
echo -- creating documentation
cd "%BASE_DIR%"
call _build\Scripts\build_doc.cmd

REM -- copy files
echo -- copying files
cd "%BASE_DIR%"
call _build\Scripts\create_build_dir.cmd
call _build\Scripts\Ipponboard_copy_files.cmd


REM -- build setup
echo -- building setup
call _build\Scripts\build_setup.cmd

REM -- done?
pause