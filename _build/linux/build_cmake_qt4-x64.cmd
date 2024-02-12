#!/usr/bin/bash
#set -x

##################
function pause(){
   read -p "$*"
}

######################
function create_env {
  LOCAL_CONFIG=_env_cfg-x64

  if [ -f $LOCAL_CONFIG ]
  then
    source $LOCAL_CONFIG
    echo using QTDIR=$QTDIR
  else
    echo create file=$LOCAL_CONFIG
    echo export QTDIR="/home/ralf/dev/inst/qt/qt-4.8.7-x64-gcc">>$LOCAL_CONFIG
    echo export PATH="/home/ralf/dev/inst/qt/qt-4.8.7-x64-gcc/bin:$PATH">>$LOCAL_CONFIG

    echo adapt the values and restart this script
    exit 0
  fi
}

######################
function make_qt_res {
  source _create_versioninfo.linux $BASE_DIR/base || exit 1

  # Create the QT resource file
  rcc -name ipponboard $BASE_DIR/base/ipponboard.qrc -o $BASE_DIR/base/qrc_ipponboard.cpp || exit 1

  echo make_qt_res finished, hit enter to continue
}

#######################
function make_release {
  CC=$(which gcc) CXX=$(which g++) cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -DCMAKE_BUILD_TYPE=Release || exit 1

  echo make_release finished, hit enter to continue
}

########################
function build_release {
  cmake --build ../_build_cmake_qt4 --target Ipponboard || exit 1
  source _copy_files ../.. -release || exit 1
  BIN_DIR=$BASE_DIR/_build/bin/Release

  make_tests

  echo build_release finished, hit enter to continue
}

######################
function build_all_release {
  make_qt_res
  make_release
  build_release

  echo build_all_release finished, hit enter to continue
}

#####################
function make_debug {
  CC=$(which gcc) CXX=$(which g++) cmake -S "../_cmake_qt4" -B "../_build_cmake_qt4" -DCMAKE_BUILD_TYPE=Debug || exit 1

  echo make_debug finished, hit enter to continue
}


######################
function build_debug {
  cmake --build ../_build_cmake_qt4 --target Ipponboard_Debug || exit 1
  source _copy_files ../.. -debug || exit 1
  BIN_DIR=$BASE_DIR/_build/bin/Debug

  echo build_debug finished, hit enter to continue
}

################
function clean {
  if [ ! -z "$BIN_DIR" ]
  then
    rm -rf ../_build_cmake_qt4
    rm -rf $BIN_DIR
  fi

  echo clean finished, hit enter to continue
}

##############
function run {
  if [ ! -z "$BIN_DIR" ]
  then
    echo switching to directory [$BIN_DIR]...
    pushd "$BIN_DIR"
    LD_LIBRARY_PATH=. ./Ipponboard
    popd
  fi

  echo run finished, hit enter to continue
}

##############
function make_tests {
  # TODO
  cmake --build ../_build_cmake_qt4 --target IpponboardTest || exit 1

  pushd ../_build_cmake_qt4/test
  ./IpponboardTest
  popd

  echo make_tests finished, hit enter to continue
}

###############
function quit {
  exit 0
}
###############
function menu {
  echo "select the operation ************"
  echo "  1) Make QT resource file"
  echo "  2) Make Release Makefiles"
  echo "  3) Build Release"
  echo "  4) Build All Release"
  echo "  5) Make Debug Makefiles"
  echo "  6) Build Debug"
  echo "  7) Clean"
  echo "  8) Run"
  echo "  t) Make Tests"
  echo "  q) Quit"

  read n
  case $n in
    1) make_qt_res;pause;clear;menu;;
    2) make_release;pause;clear;menu;;
    3) build_release;pause;clear;menu;;
    4) build_all_release;pause;clear;menu;;
    5) make_debug;pause;clear;menu;;
    6) build_debug;pause;clear;menu;;
    7) clean;pause;clear;menu;;
    8) run;pause;clear;menu;;
    t) make_tests;pause;clear;menu;;
    q) quit;;
    *) clear;menu;;
  esac
}

#####################################################################################################################
# main block
#####################################################################################################################
create_env

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

clear
menu
