#!/bin/bash

function check_cmake {
    if ! command -v cmake &> /dev/null
    then
        echo "CMake not found. Please install CMake and make sure it is in the PATH."
        exit 1
    fi
}

function init_environment {

	LOCAL_CONFIG="$PWD/env_cfg.bat"
	if [ -f "$LOCAL_CONFIG" ]; then
		source "$LOCAL_CONFIG"
	else
		echo "set \"QTDIR_LINUX=/usr/local/Qt5.15.13\"" >> "$LOCAL_CONFIG"
		echo "set \"BOOST_DIR_LINUX=/home/user/devtools/boost_1_81_0\"" >> "$LOCAL_CONFIG"
		echo "Please configure dependency paths in \"$LOCAL_CONFIG\" first!"
		read -p "Press enter to continue"
		exit 1
	fi

    if [ $? -ne 0 ]; then exit $?; fi
    read_env_cfg
    export CONFIG="release"
    export IPPONBOARD_ROOT_DIR="$PWD"
    export QTDIR="$LINUX_QTDIR"
    export BOOST_DIR="$LINUX_BOOST_DIR"
    export BUILD_DIR="$IPPONBOARD_ROOT_DIR/_build/Ipponboard-Linux"
    export BIN_DIR="$IPPONBOARD_ROOT_DIR/_bin/Ipponboard-$CONFIG"
    export TEST_BIN_DIR="$IPPONBOARD_ROOT_DIR/_bin/Test-$CONFIG"
}

function read_env_cfg {
    while IFS='=' read -r key value
    do
        export $key="$value"
    done < <(grep -o 'set ".*=.*"' ./env_cfg.bat | sed 's/set "//g' | sed 's/"//g')
}

function show_menu {
    clear

    echo "
    Current config ($CONFIG):

        QTDIR     : $QTDIR
        BOOST_DIR : $BOOST_DIR
        ROOT_DIR  : $IPPONBOARD_ROOT_DIR
        BUILD_DIR : $BUILD_DIR
        BIN_DIR   : $BIN_DIR

    Select build mode:

        (1) clean ALL
        (2) create makefiles
        (3) tests only
        (4) build all
        (5) run Ipponboard
        (6) build doc
        (7) translate resources
        (8) make tgz
        (9) clean build with tgz (release only)
        (s) switch debug/release
        (q) quit
    "
}

function execute_and_measure {
    local function=$1

    start_time=$(date +%s)
    $function
    end_time=$(date +%s)

    elapsed_time=$(expr $end_time - $start_time)
    echo "Elapsed time: $elapsed_time seconds"
    read -p "Press enter to continue"
}

function main_loop {
    init_environment

    while true; do
        show_menu
        read -n 1 -s choice
        case $choice in
            '1') execute_and_measure clean_all ;;
            '2') execute_and_measure create_makefiles ;;
            '3') execute_and_measure build_and_run_tests ;;
            '4') execute_and_measure build_all ;;
            '5') execute_and_measure run ;;
            '6') execute_and_measure build_doc ;;
            '7') execute_and_measure translate_resources ;;
            '8') execute_and_measure make_tgz ;;
            '9') execute_and_measure clean_build_with_setup ;;
            's') switch_config ;;
            'q') break ;;
            *) echo "Invalid choice" ;;
        esac
    done
}

function clean_all {
    # remove _bin and _build directories
    dirs=("$IPPONBOARD_ROOT_DIR/_bin" "$IPPONBOARD_ROOT_DIR/_build" "$IPPONBOARD_ROOT_DIR/_output")
    for item in "${dirs[@]}"; do
        echo "Removing $item"
        if [ -d "$item" ]; then
            rm -rf "$item"
        fi
    done

    echo "Removing versioninfo.h"
    rm -f "$IPPONBOARD_ROOT_DIR/base/versioninfo.h"
}

function create_makefiles {
    ./scripts/create-versioninfo.sh "$IPPONBOARD_ROOT_DIR/base" || return 1

    cmake -S $PWD -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=$CONFIG -G "Unix Makefiles" --fresh
    return $?
}

function run_tests {
    exe="$TEST_BIN_DIR/IpponboardTest"
    if [ ! -f "$exe" ]; then
        echo "Test app not found: $exe"
        return 1
    fi
    pushd "$TEST_BIN_DIR" > /dev/null
    "./IpponboardTest"
    success=$?
    popd > /dev/null
    return $success
}

function build_and_run_tests {
    NUM_CORES=$(nproc)
    cmake --build "$BUILD_DIR" --config $CONFIG --target IpponboardTest -j"$NUM_CORES" || return 1

    run_tests
    return $?
}

function build_all {
    NUM_CORES=$(nproc)
    cmake --build "$BUILD_DIR" --config $CONFIG -j"$NUM_CORES" || return 1

    run_tests || return $?
    build_doc || return $?
    return 0
}

function run {
    exe="$BIN_DIR/Ipponboard"
    if [ ! -f "$exe" ]; then
        echo "App not found: $exe"
        return 1
    fi
    pushd "$BIN_DIR" > /dev/null
    "./Ipponboard"
    success=$?
    popd > /dev/null
    return $success
}

function build_doc {
    if ! command -v pandoc &> /dev/null; then
    echo "ERROR: Pandoc not found!"
    echo "Pandoc is required to build the html help. Please install pandoc and make sure it is in the PATH."
    return 1
    fi

    echo "Creating Docs..."
    BASE_DIR="$IPPONBOARD_ROOT_DIR/doc"
    pandoc -s "$BASE_DIR/USER_MANUAL-DE.md" -o "$BIN_DIR/Anleitung.html" --metadata=title:Anleitung --css="$BASE_DIR/Ipponboard.css" --resource-path="$BASE_DIR" --self-contained || return $?
    pandoc -s "$BASE_DIR/USER_MANUAL-EN.md" -o "$BIN_DIR/User-Manual.html" --metadata=title:"User Manual" --css="$BASE_DIR/Ipponboard.css" --resource-path="$BASE_DIR" --self-contained || return $?
    pandoc -s "CHANGELOG.md" -o "$BIN_DIR/CHANGELOG.html" --css="$BASE_DIR/Ipponboard.css" --self-contained || return $?

    echo "Copying license files..."
    cp -r "$BASE_DIR/licenses" "$BIN_DIR/licenses" || return $?
    echo "done."
    return 0
}

function translate_resources {
    echo "not iplemented yet"
    return 1
}

function make_tgz {
    echo "not implemented yet"
    return 1
    #  TAG=$(git describe --tags --exact-match)
    #  if [ -z "$TAG" ];then
    #    ARCHIVE_NAME=Ipponboard-"$REV"_"$QT_VERSION"_"$DIST"-"$ARCH"
    #  else
    #    ARCHIVE_NAME=Ipponboard-"$TAG"_"$QT_VERSION"_"$DIST"-"$ARCH"
    #  fi
    #  OLD_FOLDER=$(basename "$BIN_DIR")
    #
    #  pushd "$BIN_DIR/.."
    #
    #  mv $OLD_FOLDER $ARCHIVE_NAME
    #  tar -zcvf $ARCHIVE_NAME.tgz $ARCHIVE_NAME
    #  rm -rf $ARCHIVE_NAME
    #
    #  popd
    #
}

function clean_build_with_setup {
    echo "not implemented yet"
    return 1
}

function switch_config {
    if [ "$CONFIG" == "release" ]; then
        export CONFIG="debug"
    else
        export CONFIG="release"
    fi

    export BIN_DIR="$IPPONBOARD_ROOT_DIR/_bin/Ipponboard-$CONFIG"
    export TEST_BIN_DIR="$IPPONBOARD_ROOT_DIR/_bin/Test-$CONFIG"
}

# Main
check_cmake
main_loop