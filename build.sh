#!/bin/bash

function check_cmake {
    if ! command -v cmake &> /dev/null
    then
        echo "CMake not found. Please install CMake and make sure it is in the PATH."
        exit 1
    fi
}

function check_ninja {
    if ! command -v ninja &> /dev/null
    then
        echo "Ninja not found. Please install Ninja (https://ninja-build.org/) and make sure it is in the PATH."
        exit 1
    fi
}

function check_lld {
    if command -v ld.lld &> /dev/null || command -v lld &> /dev/null
    then
        return 0
    fi

    echo "LLVM lld linker not found. Please install lld (https://lld.llvm.org/) and make sure it is in the PATH."
    exit 1
}

function verify_formatting {
    local script_dir
    script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
    local root="${IPPONBOARD_ROOT_DIR:-$script_dir}"
    "$root/scripts/check-format.sh"
    return $?
}

function init_environment {

    check_cmake
    check_ninja
    check_lld

	LOCAL_CONFIG="$PWD/env_cfg.bat"
	if [ -f "$LOCAL_CONFIG" ]; then
		source "$LOCAL_CONFIG"
	else
		echo "set \"LINUX_QTDIR=\$HOME/Qt/6.9.2/gcc_64\"" >> "$LOCAL_CONFIG"
		echo "set \"LINUX_BOOST_DIR=/home/user/devtools/boost_1_81_0\"" >> "$LOCAL_CONFIG"
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
    export OUTPUT_DIR="$IPPONBOARD_ROOT_DIR/_output"
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
        (8) make archive
        (9) clean build; make archive (release only)
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
            '8') execute_and_measure make_archive ;;
            '9') execute_and_measure clean_build_with_archive ;;
            's') switch_config ;;
            'q') break ;;
            *) echo "Invalid choice" ;;
        esac
    done
}

function clean_all {
    if [ -d "$BUILD_DIR" ]; then
        echo "Cleaning build outputs in $BUILD_DIR"
        if ! cmake --build "$BUILD_DIR" --config $CONFIG --target clean; then
            echo "WARN: CMake clean failed for $BUILD_DIR (continuing)."
        fi
    else
        echo "Build directory not found: $BUILD_DIR (skipping CMake clean)."
    fi

    echo "Removing binary output directories"
    dirs=("$IPPONBOARD_ROOT_DIR/_bin" "$OUTPUT_DIR")
    for item in "${dirs[@]}"; do
        if [ -d "$item" ]; then
            echo "  rm -rf $item"
            rm -rf "$item"
        fi
    done

    echo "Removing versioninfo.h"
    rm -f "$IPPONBOARD_ROOT_DIR/base/versioninfo.h"
}

function create_makefiles {
    ./scripts/create-versioninfo.sh "$IPPONBOARD_ROOT_DIR/base" || return 1

    cmake -S $PWD -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=$CONFIG \
        -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld" \
        -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=lld" \
        -DCMAKE_MODULE_LINKER_FLAGS="-fuse-ld=lld" \
        -G "Ninja" --fresh
    return $?
}

function run_tests {
    exe="$TEST_BIN_DIR/IpponboardTest"
    if [ ! -f "$exe" ]; then
        echo "Test app not found: $exe"
        return 1
    fi
    pushd "$TEST_BIN_DIR" > /dev/null
    QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-offscreen} \
        QT_LOGGING_RULES=${QT_LOGGING_RULES:-qt.multimedia.symbolsresolver=false} \
        IPPONBOARD_ENABLE_NETWORK_TESTS=${IPPONBOARD_ENABLE_NETWORK_TESTS:-1} ./IpponboardTest
    success=$?
    popd > /dev/null
    return $success
}

function build_and_run_tests {
    verify_formatting || return 1
    NUM_CORES=$(nproc)
    cmake --build "$BUILD_DIR" --config $CONFIG --target IpponboardTest -j"$NUM_CORES" || return 1

    run_tests
    return $?
}

function build_all {
    verify_formatting || return 1
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
    read -p "Press enter to continue"

    mkdir -p "$BIN_DIR/lang"
    "$QTDIR/bin/lrelease" -compress "$PWD/i18n/de.ts" -qm "$BIN_DIR/lang/de.qm" || return $?
    "$QTDIR/bin/lrelease" -compress "$PWD/i18n/nl.ts" -qm "$BIN_DIR/lang/nl.qm" || return $?
    return 0
}

function make_archive {
    ARCH=$(uname -m)
    SYSTEM=$(uname -s)
    RELEASE=$(uname -r)
    
    # Ipponboard-Release-Linux-x86_64-5.15.153.1-microsoft-standard-WSL2
    ARCHIVE_NAME="$OUTPUT_DIR/Ipponboard-$SYSTEM-$ARCH-$RELEASE-$CONFIG.7z"
    if [ -f "$ARCHIVE_NAME" ]; then
        rm "$ARCHIVE_NAME"
    fi
    echo "Creating archive $ARCHIVE_NAME"
    7z a "$ARCHIVE_NAME" "$BIN_DIR/*" -bso0 -bsp1 || return $?
    ARCHIVESIZE=$(du -b "$ARCHIVE_NAME" | awk '{print $1}')
    echo "Archive created with $ARCHIVESIZE bytes"
    return $?
}

function clean_build_with_archive {
    
    if [ "$CONFIG" != "release" ]; then
        switch_config
    fi
    clean_all || return $?
    create_makefiles || return $?
    build_all || return $?
    make_archive || return $?
    
    return 0
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
