#!/usr/bin/bash
#set -x

function pause(){
   read -p "$*"
}

################
# create env_cfg
################
LOCAL_CONFIG=_env_cfg-x64

if [ -f $LOCAL_CONFIG ] 
then
  source $LOCAL_CONFIG
  echo using QTDIR=$QTDIR
else
  echo create file=$LOCAL_CONFIG
  echo export QTDIR="/home/ralf/dev/inst/qt/qt-4.8.7-x64-gcc">>$LOCAL_CONFIG
  #echo export PATH="/home/ralf/dev/inst/qt/qt-4.8.7-x64-gcc/bin;$PATH;">>$LOCAL_CONFIG
 
  echo adapt the values and restart this script
  exit 1
fi

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

source _create_versioninfo.linux $BASE_DIR/base

# Create the QT resource file
rcc -name ipponboard $BASE_DIR/base/ipponboard.qrc -o $BASE_DIR/base/qrc_ipponboard.cpp 

###############
# build Release
###############
#CC=$(which gcc) CXX=$(which g++) cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -DCMAKE_BUILD_TYPE=Release
#cmake --build ../_build_cmake_qt4 --target Ipponboard
#source _copy_files ../.. -release
#cmake --build ../_build_cmake_qt4 --target setup

############
# build Debug
############
CC=$(which gcc) CXX=$(which g++) cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -DCMAKE_BUILD_TYPE=Debug
#cmake --build ../_build_cmake_qt4 --target Ipponboard_Debug
#source _copy_files ../.. -debug

echo hit any key to finish
pause

