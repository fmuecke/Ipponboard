#!/usr/bin/bash
#set -x

function pause(){
   read -p "$*"
}

################
# create env_cfg
################
LOCAL_CONFIG=_env_cfg-x64.cmd

if [ -f "$LOCAL_CONFIG" ] 
then
  source "$LOCAL_CONFIG"
else
  echo create file=$LOCAL_CONFIG
  echo QTDIR="/usr/lib64/qt4">>"$LOCAL_CONFIG"
fi

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

source _create_versioninfo.linux $BASE_DIR/base

#:: Create the QT resource file
#rcc.exe -name ipponboard $BASE_DIR/base/ipponboard.qrc -o $BASE_DIR/base/qrc_ipponboard.cpp || exit 1

#:: now build
#cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -G "Visual Studio 17 2022" -A x64
#cmake --build ../_build_cmake_qt4 --config Release --target Ipponboard
#cmake --build ../_build_cmake_qt4 --target setup 

pause

