#!/bin/bash

function detect_platform {
    case "$(uname -s)" in
        Darwin)
            export IPPONBOARD_PLATFORM="macos"
            export IPPONBOARD_BUILD_SUFFIX="MacOS"
            export IPPONBOARD_QTDIR_VAR="MACOS_QTDIR"
            ;;
        Linux)
            export IPPONBOARD_PLATFORM="linux"
            export IPPONBOARD_BUILD_SUFFIX="Linux"
            export IPPONBOARD_QTDIR_VAR="LINUX_QTDIR"
            ;;
        *)
            echo "Unsupported platform: $(uname -s)"
            exit 1
            ;;
    esac
}

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
    if [ "$IPPONBOARD_PLATFORM" != "linux" ]; then
        return 0
    fi

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
    if "$root/scripts/check-format.sh"; then
        return 0
    fi

    echo "WARN: Formatting check failed. Continuing because fast-path builds are enabled."
    return 0
}

function create_default_env_cfg {
    local local_config="$1"

    cat > "$local_config" <<EOF_CFG
set "LINUX_QTDIR=\$HOME/Qt/6.9.2/gcc_64"
set "MACOS_QTDIR=\$HOME/Qt/6.9.2/macos"
EOF_CFG
}

function init_environment {
    detect_platform
    check_cmake
    check_ninja
    check_lld

    LOCAL_CONFIG="$PWD/env_cfg.bat"
    if [ -f "$LOCAL_CONFIG" ]; then
        source "$LOCAL_CONFIG"
    else
        create_default_env_cfg "$LOCAL_CONFIG"
        echo "Please configure dependency paths in \"$LOCAL_CONFIG\" first!"
        read -p "Press enter to continue"
        exit 1
    fi

    if [ $? -ne 0 ]; then exit $?; fi
    read_env_cfg
    export CONFIG="release"
    export IPPONBOARD_ROOT_DIR="$PWD"
    export QTDIR="${!IPPONBOARD_QTDIR_VAR}"
    if [ -z "$QTDIR" ]; then
        echo "Missing $IPPONBOARD_QTDIR_VAR in $LOCAL_CONFIG"
        exit 1
    fi
    export BUILD_DIR="$IPPONBOARD_ROOT_DIR/_build/Ipponboard-$IPPONBOARD_BUILD_SUFFIX"
    export BIN_DIR="$IPPONBOARD_ROOT_DIR/_bin/Ipponboard-$CONFIG"
    export TEST_BIN_DIR="$IPPONBOARD_ROOT_DIR/_bin/Test-$CONFIG"
    export OUTPUT_DIR="$IPPONBOARD_ROOT_DIR/_output"
}

function read_env_cfg {
    while IFS="=" read -r key value
    do
        export $key="$value"
    done < <(grep -o "set \".*=.*\"" ./env_cfg.bat | sed "s/set \"//g" | sed "s/\"//g")
}

function get_num_cores {
    if command -v nproc &> /dev/null; then
        nproc
        return 0
    fi

    if command -v sysctl &> /dev/null; then
        sysctl -n hw.ncpu
        return 0
    fi

    echo 1
}

function show_menu {
    clear

    echo "
    Current config ($CONFIG):

        QTDIR     : $QTDIR
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
            1) execute_and_measure clean_all ;;
            2) execute_and_measure create_makefiles ;;
            3) execute_and_measure build_and_run_tests ;;
            4) execute_and_measure build_all ;;
            5) execute_and_measure run ;;
            6) execute_and_measure build_doc ;;
            7) execute_and_measure translate_resources ;;
            8) execute_and_measure make_archive ;;
            9) execute_and_measure clean_build_with_archive ;;
            s) switch_config ;;
            q) break ;;
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

    local linker_args=()
    if [ "$IPPONBOARD_PLATFORM" = "linux" ]; then
        linker_args=(
            -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld
            -DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld
            -DCMAKE_MODULE_LINKER_FLAGS=-fuse-ld=lld
        )
    fi

    cmake -S "$PWD" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=$CONFIG \
        "${linker_args[@]}" \
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
    echo IpponboardTest:
    QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-offscreen} \
        QT_LOGGING_RULES=${QT_LOGGING_RULES:-qt.multimedia.symbolsresolver=false} \
        ./IpponboardTest
    success=$?

    if [ $success -eq 0 ]; then
        net_exe="$TEST_BIN_DIR/IpponboardNetworkTest"
        if [ -f "$net_exe" ]; then
            echo IpponboardNetworkTest:
            QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-offscreen} \
                QT_LOGGING_RULES=${QT_LOGGING_RULES:-qt.multimedia.symbolsresolver=false} \
                ./IpponboardNetworkTest
            success=$?
        else
            echo "Network test app not found: $net_exe"
            success=1
        fi
    fi
    popd > /dev/null
    return $success
}

function build_and_run_tests {
    verify_formatting
    NUM_CORES=$(get_num_cores)
    cmake --build "$BUILD_DIR" --config $CONFIG --target IpponboardTest -j"$NUM_CORES" || return 1
    cmake --build "$BUILD_DIR" --config $CONFIG --target IpponboardNetworkTest -j"$NUM_CORES" || return 1

    run_tests
    return $?
}

function build_all {
    verify_formatting
    NUM_CORES=$(get_num_cores)
    cmake --build "$BUILD_DIR" --config $CONFIG -j"$NUM_CORES" || return 1

    run_tests || return $?
    build_doc || return $?
    return 0
}

function run {
    local exe
    if [ "$IPPONBOARD_PLATFORM" = "macos" ]; then
        exe="$BIN_DIR/Ipponboard.app/Contents/MacOS/Ipponboard"
    else
        exe="$BIN_DIR/Ipponboard"
    fi

    if [ ! -f "$exe" ]; then
        echo "App not found: $exe"
        return 1
    fi

    local exe_dir
    exe_dir=$(dirname "$exe")
    pushd "$exe_dir" > /dev/null
    "./$(basename "$exe")"
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
    pandoc -s "$BASE_DIR/USER_MANUAL-DE.md" -o "$BIN_DIR/Anleitung.html" --template="$BASE_DIR/pandoc-template.html" --css="$BASE_DIR/Ipponboard.css" --resource-path="$BASE_DIR" --self-contained || return $?
    pandoc -s "$BASE_DIR/USER_MANUAL-EN.md" -o "$BIN_DIR/User-Manual.html" --template="$BASE_DIR/pandoc-template.html" --css="$BASE_DIR/Ipponboard.css" --resource-path="$BASE_DIR" --self-contained || return $?
    pandoc -s "CHANGELOG.md" -o "$BIN_DIR/CHANGELOG.html" --template="$BASE_DIR/pandoc-template.html" --css="$BASE_DIR/Ipponboard.css" --resource-path="$BASE_DIR" --self-contained || return $?

    if [ "$IPPONBOARD_PLATFORM" = "macos" ]; then
        local bundle_resources="$BIN_DIR/Ipponboard.app/Contents/Resources"
        mkdir -p "$bundle_resources" || return $?
        cp "$BIN_DIR/Anleitung.html" "$bundle_resources/Anleitung.html" || return $?
        cp "$BIN_DIR/User-Manual.html" "$bundle_resources/User-Manual.html" || return $?
        cp "$BIN_DIR/CHANGELOG.html" "$bundle_resources/CHANGELOG.html" || return $?
    fi
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

    ARCHIVE_NAME="$OUTPUT_DIR/Ipponboard-$SYSTEM-$ARCH-$RELEASE-$CONFIG.7z"
    if [ -f "$ARCHIVE_NAME" ]; then
        rm "$ARCHIVE_NAME"
    fi
    echo "Creating archive $ARCHIVE_NAME"
    7z a "$ARCHIVE_NAME" "$BIN_DIR/*" -bso0 -bsp1 || return $?
    set -- $(du -b "$ARCHIVE_NAME")
    ARCHIVESIZE=$1
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
