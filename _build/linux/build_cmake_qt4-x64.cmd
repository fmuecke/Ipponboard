#!/usr/bin/bash

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
  source "$LOCAL_CONFIG"
fi

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

source _create_versioninfo.linux $BASE_DIR/base

#:: Create the QT resource file
rcc -name ipponboard $BASE_DIR/base/ipponboard.qrc -o $BASE_DIR/base/qrc_ipponboard.cpp 

#:: now build
cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4"
cmake --build ../_build_cmake_qt4 --config Release --target Ipponboard
#cmake --build ../_build_cmake_qt4 --config Debug --target Ipponboard_Debug
#cmake --build ../_build_cmake_qt4 --target setup 

echo hit any key to finish
pause

