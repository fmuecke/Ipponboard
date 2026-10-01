#!/bin/bash

# Generates the local dependency configuration (env_cfg.bat).
#
# Usage: init_env_cfg.sh <deb|rh|macos|home|ud> [output-file]
#
# Profiles:
#   deb   Linux with Debian/Ubuntu Qt packages (QTDIR=/usr/lib/qt6)
#   rh    Linux with Red Hat/Fedora Qt packages (QTDIR=/usr/lib64/qt6)
#   macos macOS with Homebrew Qt (QTDIR=/usr/local/opt/qt)
#   home  latest Qt installation below $HOME/Qt via symlink $HOME/Qt/latest
#   ud    user defined; prompts for QTDIR and IPPONBOARD_ROOT_DIR and
#         suggests defaults (QTDIR per detected package manager)
#
# The profile is mandatory; the output file is overwritten.
# IPPONBOARD_ROOT_DIR defaults to the directory of the output file.
# Paths are written fully expanded, because the consumers of env_cfg.bat
# do not expand variables in the values.

set -u

function detect_platform {
    case "$(uname -s)" in
        Darwin)
            PLATFORM_NAME="macos"
            QTDIR_VAR="MACOS_QTDIR"
            QT_KIT_DIR="macos"
            ;;
        Linux)
            PLATFORM_NAME="linux"
            QTDIR_VAR="LINUX_QTDIR"
            QT_KIT_DIR="gcc_64"
            ;;
        *)
            echo "ERROR: unsupported platform: $(uname -s)" >&2
            return 1
            ;;
    esac
    return 0
}

function write_env_cfg {
    local output="$1"
    local qtdir="$2"
    local root_dir="$3"

    cat > "$output" <<EOF_CFG
set "IPPONBOARD_ROOT_DIR=$root_dir"
set "$QTDIR_VAR=$qtdir"
EOF_CFG
}

# Resolves the latest Qt installation below $HOME/Qt and points the
# symlink $HOME/Qt/latest at its version directory.
function resolve_home_qtdir {
    local qt_base="$HOME/Qt"
    local latest_version
    latest_version=$(find "$qt_base" -mindepth 1 -maxdepth 1 -type d -name '[0-9]*' 2>/dev/null | sort -V | tail -n 1)

    if [ -z "$latest_version" ]; then
        echo "ERROR: no Qt version directory found below $qt_base" >&2
        return 1
    fi

    if ! ln -sfn "$latest_version" "$qt_base/latest"; then
        echo "ERROR: failed to create symlink $qt_base/latest" >&2
        return 1
    fi

    local qtdir="$qt_base/latest/$QT_KIT_DIR"
    if [ ! -d "$qtdir" ]; then
        echo "ERROR: kit directory not found below $latest_version (expected $QT_KIT_DIR)" >&2
        return 1
    fi

    echo "$qtdir"
    return 0
}

# Suggests a QTDIR based on the package manager in use:
# dnf -> /usr/lib64/qt6, apt -> /usr/lib/qt6, Homebrew -> Qt prefix.
function detect_default_qtdir {
    if command -v dnf > /dev/null 2>&1; then
        echo "/usr/lib64/qt6"
    elif command -v apt > /dev/null 2>&1; then
        echo "/usr/lib/qt6"
    elif command -v brew > /dev/null 2>&1; then
        local brew_qtdir
        brew_qtdir=$(brew --prefix qt 2> /dev/null)
        echo "${brew_qtdir:-/usr/local/opt/qt}"
    else
        return 1
    fi
    return 0
}

function main {
    local profile="${1:-}"
    local output="${2:-$PWD/env_cfg.bat}"
    local root_dir
    root_dir=$(cd "$(dirname "$output")" && pwd)

    if [ -z "$profile" ]; then
        echo "ERROR: missing profile (usage: init_env_cfg.sh <deb|rh|macos|home|ud> [output-file])" >&2
        return 1
    fi

    detect_platform || return 1

    local qtdir=""
    local user_root=""
    case "$profile" in
        deb)
            if [ "$PLATFORM_NAME" != "linux" ]; then
                echo "ERROR: profile 'deb' requires Linux (detected: $PLATFORM_NAME)" >&2
                return 1
            fi
            qtdir="/usr/lib/qt6"
            ;;
        rh)
            if [ "$PLATFORM_NAME" != "linux" ]; then
                echo "ERROR: profile 'rh' requires Linux (detected: $PLATFORM_NAME)" >&2
                return 1
            fi
            qtdir="/usr/lib64/qt6"
            ;;
        macos)
            if [ "$PLATFORM_NAME" != "macos" ]; then
                echo "ERROR: profile 'macos' requires macOS (detected: $PLATFORM_NAME)" >&2
                return 1
            fi
            qtdir="/usr/local/opt/qt"
            ;;
        home)
            qtdir=$(resolve_home_qtdir) || return 1
            ;;
        ud)
            local default_qtdir=""
            local input_qtdir=""
            default_qtdir=$(detect_default_qtdir) || true
            if [ -n "$default_qtdir" ]; then
                read -r -p "QTDIR [$default_qtdir]: " input_qtdir
                qtdir="${input_qtdir:-$default_qtdir}"
            else
                read -r -p "QTDIR (Qt installation path): " qtdir
            fi
            if [ -z "$qtdir" ] || [ ! -d "$qtdir" ]; then
                echo "ERROR: not a directory: $qtdir" >&2
                return 1
            fi
            read -r -p "IPPONBOARD_ROOT_DIR [$root_dir]: " user_root
            root_dir="${user_root:-$root_dir}"
            if [ ! -d "$root_dir" ]; then
                echo "ERROR: not a directory: $root_dir" >&2
                return 1
            fi
            ;;
        *)
            echo "ERROR: unknown profile '$profile' (use deb, rh, macos, home or ud)" >&2
            return 1
            ;;
    esac

    if [ ! -d "$qtdir" ]; then
        echo "WARN: Qt directory not found: $qtdir" >&2
    fi

    write_env_cfg "$output" "$qtdir" "$root_dir"
    echo "Created $output (profile: $profile)"
    return 0
}

if [ "${BASH_SOURCE[0]}" = "$0" ]; then
    main "$@"
fi
