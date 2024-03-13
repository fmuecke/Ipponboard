#!/usr/bin/bash
#set -x

#########################
function create_env_qt4 {
  LOCAL_CONFIG=_env_cfg-$ARCH

  if [ -f $LOCAL_CONFIG ]; then
    source $LOCAL_CONFIG
    echo using QTDIR=$QTDIR
  else
    echo create file=$LOCAL_CONFIG
    echo export QTDIR="~/dev/inst/qt/qt-4.8.7-$ARCH-gcc">>$LOCAL_CONFIG
    echo export PATH="~/dev/inst/qt/qt-4.8.7-$ARCH-gcc/bin:$PATH">>$LOCAL_CONFIG
    echo adapt the values and restart this script
    exit 0
  fi
}

#########################
function create_env_qt5 {
  LOCAL_CONFIG=_env_cfg-$ARCH

  if [ -f $LOCAL_CONFIG ]; then
    source $LOCAL_CONFIG
    echo using QTDIR=$QTDIR
  else
    echo create file=$LOCAL_CONFIG

    if [ $DIST == "deb" ]; then
      echo \# debian qt path>>$LOCAL_CONFIG
      if [ $ARCH == "x64" ]; then
        QT_OS_DIR="/usr/lib/x86_64-linux-gnu"
      elif [ $ARCH == "x32" ]; then
        QT_OS_DIR="/usr/lib/i386-linux-gnu"
      fi
    elif [ $DIST == "rh" ]; then
      echo \# redhat qt path>>$LOCAL_CONFIG
      if [ $ARCH == "x64" ]; then
          QT_OS_DIR="/usr/lib64"
      fi
    fi
    echo export QT_OS_DIR=$QT_OS_DIR>>$LOCAL_CONFIG

    if [ "$QT_DEFAULT_DIR" == true ]; then
      echo export QTDIR="$QT_OS_DIR">>$LOCAL_CONFIG
      echo export PATH="$QT_OS_DIR/qt5/bin:$PATH">>$LOCAL_CONFIG
    else
      echo \# local qt path>>$LOCAL_CONFIG
      echo export QTDIR="~/dev/inst/qt/qt-5.15.2-$ARCH-gcc-dbg">>$LOCAL_CONFIG
      echo export PATH="~/dev/inst/qt/qt-5.15.2-$ARCH-gcc-dbg/bin:$PATH">>$LOCAL_CONFIG
    fi

    if [ "$AUTOBUILD" == true ]; then
      source $LOCAL_CONFIG
      echo using QTDIR=$QTDIR
    else
      echo adapt the values and restart this script
      exit 0
    fi
  fi
}

######################
function make_qt_res {
  echo --[make_qt_res]--
  source _create_versioninfo.linux $BASE_DIR/base || exit 1

  # Create the QT resource file
  rcc -name ipponboard $BASE_DIR/base/ipponboard.qrc -o $BASE_DIR/base/qrc_ipponboard.cpp || exit 1

  echo make_qt_res finished, hit enter to continue
}

#######################
function make_release {
  echo --[make_release]--
  CC=$(which gcc) CXX=$(which g++) cmake -S "../_cmake_$QT_VERSION" -B "../_build_cmake_$QT_VERSION" -DCMAKE_BUILD_TYPE=Release || exit 1

  echo make_release finished, hit enter to continue
}

########################
function build_release {
  echo --[build_release]--
  make_tests || exit 1

  cmake --build ../_build_cmake_$QT_VERSION --target Ipponboard || exit 1
  source _copy_files ../.. -release || exit 1
  source _copy_files_$QT_VERSION ../.. -release || exit 1

  BIN_DIR=$BASE_DIR/_build/bin/Release
}

############################
function build_all_release {
  echo --[build_all_release]--
  clean
  make_qt_res
  make_release
  build_release
  deploy_without_qt

  echo build_all_release finished, hit enter to continue
}

####################################
function build_all_release_with_qt {
  echo --[build_all_release_with_qt]--

  build_all_release
  deploy_qt_standalone

  echo build_all_release_with_qt finished, hit enter to continue
}

#####################
function make_debug {
  echo --[make_debug]--
  CC=$(which gcc) CXX=$(which g++) cmake -S "../_cmake_$QT_VERSION" -B "../_build_cmake_$QT_VERSION" -DCMAKE_BUILD_TYPE=Debug || exit 1

  echo make_debug finished, hit enter to continue
}


######################
function build_debug {
  echo --[build_debug]--
  cmake --build ../_build_cmake_$QT_VERSION --target Ipponboard_Debug || exit 1
  source _copy_files ../.. -debug || exit 1

  BIN_DIR=$BASE_DIR/_build/bin/Debug
  echo build_debug finished, hit enter to continue
}

################
function clean {
  echo --[make_clean]--
  if [ ! -z "$BIN_DIR" ]
  then
    rm -rf ../_build_cmake_$QT_VERSION
    rm -rf $BIN_DIR
  fi

  echo clean finished, hit enter to continue
}

##############
function run {
  echo --[run]--
  if [ ! -z "$BIN_DIR" ]
  then
    echo switching to directory [$BIN_DIR]...
    pushd "$BIN_DIR"
    LD_LIBRARY_PATH=. ./Ipponboard
    popd
  fi

  echo run finished, hit enter to continue
}

#####################
function make_tests {
  echo --[make_tests]--
  cmake --build ../_build_cmake_$QT_VERSION --target IpponboardTest || exit 1

  pushd ../_build_cmake_$QT_VERSION/test
  if ./IpponboardTest
  then
    popd
    echo make_tests sucessfully finished
  else
    popd
    echo make_tests with errors, exiting build
    exit 1
  fi
}

####################
function translate {
  echo --[make_translation]--
  pushd $BASE_DIR
  lupdate -noobsolete base core Widgets -locations relative -no-recursive -ts i18n/de.ts -ts i18n/nl.ts
  linguist i18n/de.ts i18n/nl.ts
  lrelease -compress i18n/de.ts -qm i18n/de.qm
  lrelease -compress i18n/nl.ts -qm i18n/nl.qm
  popd
}

###################
function make_tgz {
  TAG=$(git describe --tags --exact-match)
  if [ -z "$TAG" ];then
    ARCHIVE_NAME=Ipponboard-"$REV"_"$QT_VERSION"_"$DIST"-"$ARCH"
  else
    ARCHIVE_NAME=Ipponboard-"$TAG"_"$QT_VERSION"_"$DIST"-"$ARCH"
  fi
  OLD_FOLDER=$(basename "$BIN_DIR")

  pushd "$BIN_DIR/.."

  mv $OLD_FOLDER $ARCHIVE_NAME
  tar -zcvf $ARCHIVE_NAME.tgz $ARCHIVE_NAME
  rm -rf $ARCHIVE_NAME

  popd

  echo make_tgz finished, hit enter to continue
}

#################
function pause(){
   read -p "$*"
}

###############
function quit {
  exit 0
}

###############
function menu {
  echo "Architecture: $ARCH"
  echo "Qt-Version:   $QT_VERSION"
  echo "Distribution: $DIST"
  echo "Autobuild:    $AUTOBUILD"
  echo
  echo "Select the operation"
  echo "  a) Build All Release"
  echo "  b) Build All Release without QT-Libraries"
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
    a) build_all_release_with_qt;make_tgz;pause;clear;menu;;
    b) build_all_release;make_tgz;pause;clear;menu;;
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

##################
function usage() {
    echo "Verwendung: $0 --arch <x32|x64> --qt <qt4|qt5> --dist <deb|rh> [--use_QT_default_dir] [--autobuild]"
    exit 1
}

#####################################################################################################
# main block
#####################################################################################################
ARCH=""
QT_VERSION=""
DIST=""
QT_DEFAULT_DIR=false
AUTOBUILD=false

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
    --dist)
      DIST="$2"
      shift
      ;;
    --use_QT_default_dir)
      QT_DEFAULT_DIR=true
      ;;
    --autobuild)
      AUTOBUILD=true
      ;;
    *)
      echo "Ungültige Option: $1"
      usage
      ;;
  esac
  shift
done

if [ -z $ARCH ] || [ -z $QT_VERSION ] || [ -z "$DIST" ]; then
  usage
  exit 1
fi

create_env_$QT_VERSION

BIN_DIR=$BASE_DIR/_build/bin

PWD=$(pwd)
BASE_DIR=$PWD/../..
echo BASE_DIR=$BASE_DIR

clear
if [ "$AUTOBUILD" == true ]; then
  echo "Architecture: $ARCH"
  echo "Qt-Version:   $QT_VERSION"
  echo "Distribution: $DIST"
  echo "Autobuild starting ..."
  clean
  make_qt_res
  make_release
  build_release
  deploy_without_qt
  make_tgz
  echo ... autobuild finished.
else
  menu
fi
