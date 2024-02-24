#!/usr/bin/bash
#set -x

#########################
function create_env_qt4 {
  LOCAL_CONFIG=_env_cfg-$ARCH

  if [ -f $LOCAL_CONFIG ]
  then
    source $LOCAL_CONFIG
    echo using QTDIR=$QTDIR
  else
    echo create file=$LOCAL_CONFIG
    echo export QTDIR="/home/ralf/dev/inst/qt/qt-4.8.7-$ARCH-gcc">>$LOCAL_CONFIG
    echo export PATH="/home/ralf/dev/inst/qt/qt-4.8.7-$ARCH-gcc/bin:$PATH">>$LOCAL_CONFIG

    echo adapt the values and restart this script
    exit 0
  fi
}

#########################
function create_env_qt5 {
  LOCAL_CONFIG=_env_cfg-$ARCH

  if [ -f $LOCAL_CONFIG ]
  then
    source $LOCAL_CONFIG
    echo using QTDIR=$QTDIR
  else
    echo create file=$LOCAL_CONFIG
    echo export QTDIR="/home/ralf/dev/inst/qt/qt-5.15.2-$ARCH-gcc-dbg">>$LOCAL_CONFIG
    echo export PATH="/home/ralf/dev/inst/qt/qt-5.15.2-$ARCH-gcc-dbg/bin:$PATH">>$LOCAL_CONFIG
    e#cho export LD_LIBRARY_PATH=/home/ralf/dev/inst/qt/qt-5.15.2-$ARCH-gcc-dbg/lib:$LD_LIBRARY_PATH>>$LOCAL_CONFIG

    echo adapt the values and restart this script
    exit 0
  fi
}

###############
function menu {
  echo "Select the operation"
  echo "  a) Build All Release"
  echo "  1) Make QT resource file"
  echo "  2) Make Release Makefiles"
  echo "  3) Build Release"
  echo "  4) Make Debug Makefiles"
  echo "  5) Build Debug"
  echo "  6) Clean"
  echo "  7) Run"
  echo "  8) Make Tests"
  echo "  t) Make Translations"
  echo "  q) Quit"

  read n
  case $n in
    a) build_all_release;pause;clear;menu;;
    1) make_qt_res;pause;clear;menu;;
    2) make_release;pause;clear;menu;;
    3) build_release;pause;clear;menu;;
    4) make_debug;pause;clear;menu;;
    5) build_debug;pause;clear;menu;;
    6) clean;pause;clear;menu;;
    7) run;pause;clear;menu;;
    8) make_tests;pause;clear;menu;;
    t) translate;pause;clear;menu;;
    q) quit;;
    *) clear;menu;;
  esac
}

#################
function usage() {
    echo "Verwendung: $0 --arch <x32|x64> --qt <qt4|qt5>"
    exit 1
}

################################################################################################################
# main block
################################################################################################################
while [[ "$#" -gt 0 ]]; do
  case $1 in
    --arch)
      ARCH="$2"
      shift
      ;;
    --qt)
      QT_VERSION="$2"
      shift
      ;;
    *)
      echo "Ungültige Option: $1"
      usage
      ;;
  esac
  shift
done

if [[ -z $ARCH ]] || [[ -z $QT_VERSION ]]; then
  usage
  exit 1
fi

create_env_$QT_VERSION
source _common

BIN_DIR=$BASE_DIR/_build/bin

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

clear
menu
