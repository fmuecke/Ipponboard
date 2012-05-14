SET BASE_DIR=c:\dev\bitbucket\ipponboard
SET BUILD_DIR=%BASE_DIR%\_build\build_output\~tmp
SET BUILD_DIR_TEAM=%BASE_DIR%\_build\build_output\~tmp_TE

IF "%VS100COMNTOOLS%"=="" (
  call "%VS90COMNTOOLS%..\..\vc\vcvarsall.bat" x86
) ELSE (
  call "%VS100COMNTOOLS%..\..\vc\vcvarsall.bat" x86
)