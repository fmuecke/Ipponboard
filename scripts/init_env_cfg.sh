#!/bin/bash

# Generates the local dependency configuration (env_cfg.bat).
#
# Usage: init_env_cfg.sh <apt|dnf|brew|ud> [output-file]
#
# Profiles:
#   apt   Qt from Debian/Ubuntu packages (QTDIR=/usr/lib/qt6)
#   dnf   Qt from Red Hat/Fedora packages (QTDIR=/usr/lib64/qt6)
#   brew  Qt from Homebrew (QTDIR=brew --prefix qt)
#   ud    user defined; prompts for QTDIR and IPPONBOARD_ROOT_DIR and
#         suggests defaults (existing env_cfg.bat values first, then QTDIR
#         per detected package manager)
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
            ;;
        Linux)
            QTDIR_VAR="LINUX_QTDIR"
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

# Reads a value from an existing env_cfg.bat (set "KEY=VALUE" lines).
function read_cfg_value {
    local file="$1"
    local key="$2"
    local line name value

    while IFS= read -r line; do
        case "$line" in
            set\ \"*\"*)
                name="${line#set \"}"
                name="${name%%=*}"
                if [ "$name" = "$key" ]; then
                    value="${line#*=}"
                    value="${value%\"}"
                    echo "$value"
                    return 0
                fi
                ;;
        esac
    done < "$file"
    return 1
}

function main {
    local profile="${1:-}"
    local output="${2:-$PWD/env_cfg.bat}"
    local root_dir
    root_dir=$(cd "$(dirname "$output")" && pwd)

    if [ -z "$profile" ]; then
        echo "ERROR: missing profile (usage: init_env_cfg.sh <apt|dnf|brew|ud> [output-file])" >&2
        return 1
    fi

    detect_platform || return 1

    local qtdir=""
    local user_root=""
    case "$profile" in
        apt|dnf|brew)
            qtdir=$(detect_default_qtdir "$profile") || return 1
            ;;
        ud)
            local default_qtdir=""
            local input_qtdir=""
            local cfg_value=""
            default_qtdir=$(detect_default_qtdir) || true
            if [ -f "$output" ]; then
                cfg_value=$(read_cfg_value "$output" "IPPONBOARD_ROOT_DIR") || true
                root_dir="${cfg_value:-$root_dir}"
                cfg_value=$(read_cfg_value "$output" "$QTDIR_VAR") || true
                if [ -z "$cfg_value" ]; then
                    cfg_value=$(read_cfg_value "$output" "QTDIR") || true
                fi
                default_qtdir="${cfg_value:-$default_qtdir}"
            fi
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
            echo "ERROR: unknown profile '$profile' (use apt, dnf, brew or ud)" >&2
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
