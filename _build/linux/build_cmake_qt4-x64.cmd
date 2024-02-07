#!/usr/bin/bash
#set -x

function pause(){
   read -p "$*"
}

################
# create env_cfg
################
LOCAL_CONFIG=_env_cfg-x64

if [ -f "$LOCAL_CONFIG" ] 
then
  source "$LOCAL_CONFIG"
else
  echo create file=$LOCAL_CONFIG
  echo QTDIR="/usr/lib/x86_64-linux-gnu/qt4">>"$LOCAL_CONFIG"
  echo adapt the values and restart this script
  exit
fi

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

source _create_versioninfo.linux $BASE_DIR/base

# Create the QT resource file
rcc -name ipponboard $BASE_DIR/base/ipponboard.qrc -o $BASE_DIR/base/qrc_ipponboard.cpp 

#############
# build Release
#############
cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -D CMAKE_BUILD_TYPE=Release 
#CC=/usr/local/bin/gcc CXX=/usr/local/bin/g++ cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4"
cmake --build ../_build_cmake_qt4 --target Ipponboard
source _copy_files ../.. -release

############
# build Debug
############
#cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -D CMAKE_BUILD_TYPE=Debug 
#cmake --build ../_build_cmake_qt4 --target Ipponboard_Debug
#source _copy_files ../.. -debug

####
# all
####
#cmake --build ../_build_cmake_qt4 --target setup

echo hit any key to finish
pause

