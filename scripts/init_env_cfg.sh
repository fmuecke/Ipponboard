#!/bin/bash

# Generates the local dependency configuration (env_cfg.bat).
#
# Usage: init_env_cfg.sh <apt|dnf|brew|home|ud> [output-file]
#
# Profiles:
#   apt   Qt from Debian/Ubuntu packages (QTDIR=/usr/lib/qt6)
#   dnf   Qt from Red Hat/Fedora packages (QTDIR=/usr/lib64/qt6)
#   brew  Qt from Homebrew (QTDIR=brew --prefix qt)
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
            QTDIR_VAR="MACOS_QTDIR"
            QT_KIT_DIR="macos"
            ;;
        Linux)
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

# Suggests a QTDIR for the given package manager (dnf, apt, brew).
# Without argument the package manager in use is detected:
# dnf -> /usr/lib64/qt6, apt -> /usr/lib/qt6, Homebrew -> Qt prefix.
function detect_default_qtdir {
    local manager="${1:-}"

    if [ -z "$manager" ]; then
        if command -v dnf > /dev/null 2>&1; then
            manager="dnf"
        elif command -v apt > /dev/null 2>&1; then
            manager="apt"
        elif command -v brew > /dev/null 2>&1; then
            manager="brew"
        else
            return 1
        fi
    fi

    case "$manager" in
        dnf)
            echo "/usr/lib64/qt6"
            ;;
        apt)
            echo "/usr/lib/qt6"
            ;;
        brew)
            local brew_qtdir
            brew_qtdir=$(brew --prefix qt 2> /dev/null)
            echo "${brew_qtdir:-/usr/local/opt/qt}"
            ;;
        *)
            return 1
            ;;
    esac
    return 0
}

function main {
    local profile="${1:-}"
    local output="${2:-$PWD/env_cfg.bat}"
    local root_dir
    root_dir=$(cd "$(dirname "$output")" && pwd)

    if [ -z "$profile" ]; then
        echo "ERROR: missing profile (usage: init_env_cfg.sh <apt|dnf|brew|home|ud> [output-file])" >&2
        return 1
    fi

    detect_platform || return 1

    local qtdir=""
    local user_root=""
    case "$profile" in
        apt|dnf|brew)
            qtdir=$(detect_default_qtdir "$profile") || return 1
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
            echo "ERROR: unknown profile '$profile' (use apt, dnf, brew, home or ud)" >&2
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
