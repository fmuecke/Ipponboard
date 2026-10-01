#!/bin/bash

# Smoke tests for scripts/init_env_cfg.sh.
# Runs without external dependencies and removes its temp directory afterwards.
#
# Usage: scripts/test_init_env_cfg.sh

set -u

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
GENERATOR="$SCRIPT_DIR/init_env_cfg.sh"
TESTS=0
FAILURES=0

case "$(uname -s)" in
    Darwin)
        QTDIR_VAR="MACOS_QTDIR"
        KIT_DIR="macos"
        PLATFORM_NAME="macos"
        ;;
    *)
        QTDIR_VAR="LINUX_QTDIR"
        KIT_DIR="gcc_64"
        PLATFORM_NAME="linux"
        ;;
esac

function report {
    local status="$1"
    local message="$2"

    TESTS=$((TESTS + 1))
    if [ "$status" -eq 0 ]; then
        echo "PASS: $message"
    else
        echo "FAIL: $message"
        FAILURES=$((FAILURES + 1))
    fi
}

function assert_contains {
    local file="$1"
    local pattern="$2"

    if grep -qF -- "$pattern" "$file"; then
        report 0 "$file contains $pattern"
    else
        report 1 "$file contains $pattern"
    fi
}

function assert_not_contains {
    local file="$1"
    local pattern="$2"

    if grep -qF -- "$pattern" "$file"; then
        report 1 "$file does not contain $pattern"
    else
        report 0 "$file does not contain $pattern"
    fi
}

function assert_cfg_format {
    local file="$1"

    if grep -v -E '^set "[^=]+=.*"$' "$file" | grep -q .; then
        report 1 "$file matches set \"KEY=VALUE\" format"
    else
        report 0 "$file matches set \"KEY=VALUE\" format"
    fi
}

function assert_succeeds {
    local message="$1"
    shift

    if "$@" > /dev/null 2>&1; then
        report 0 "$message"
    else
        report 1 "$message"
    fi
}

function assert_fails {
    local message="$1"
    shift

    if "$@" > /dev/null 2>&1; then
        report 1 "$message"
    else
        report 0 "$message"
    fi
}

WORK_DIR=$(mktemp -d)
trap 'rm -rf "$WORK_DIR"' EXIT

REPO_DIR="$WORK_DIR/repo"
OUT="$REPO_DIR/env_cfg.bat"
mkdir -p "$REPO_DIR"

# --- a profile is mandatory
assert_fails "missing profile fails" "$GENERATOR" "" "$OUT"
assert_fails "missing arguments fail" "$GENERATOR"
if [ -f "$OUT" ]; then
    report 1 "no output file is created without a profile"
else
    report 0 "no output file is created without a profile"
fi

# --- apt and dnf map to the distro Qt paths
assert_succeeds "profile apt succeeds" "$GENERATOR" apt "$OUT"
assert_contains "$OUT" "set \"$QTDIR_VAR=/usr/lib/qt6\""
assert_contains "$OUT" "set \"IPPONBOARD_ROOT_DIR=$REPO_DIR\""
assert_cfg_format "$OUT"

assert_succeeds "profile dnf succeeds" "$GENERATOR" dnf "$OUT"
assert_contains "$OUT" "set \"$QTDIR_VAR=/usr/lib64/qt6\""
assert_contains "$OUT" "set \"IPPONBOARD_ROOT_DIR=$REPO_DIR\""

# --- legacy profile names are rejected
assert_fails "legacy profile deb fails" "$GENERATOR" deb "$OUT"
assert_fails "legacy profile macos fails" "$GENERATOR" macos "$OUT"

# --- home picks the newest version and creates the symlink
FAKE_HOME="$WORK_DIR/home"
mkdir -p "$FAKE_HOME/Qt/5.15.2/gcc_64" "$FAKE_HOME/Qt/5.15.2/macos"
mkdir -p "$FAKE_HOME/Qt/6.9.2/gcc_64" "$FAKE_HOME/Qt/6.9.2/macos"
mkdir -p "$FAKE_HOME/Qt/6.11.2/gcc_64" "$FAKE_HOME/Qt/6.11.2/macos"
assert_succeeds "profile home succeeds" env HOME="$FAKE_HOME" "$GENERATOR" home "$OUT"
if [ "$(readlink "$FAKE_HOME/Qt/latest")" = "$FAKE_HOME/Qt/6.11.2" ]; then
    report 0 "symlink Qt/latest points to 6.11.2"
else
    report 1 "symlink Qt/latest points to 6.11.2"
fi
assert_contains "$OUT" "set \"$QTDIR_VAR=$FAKE_HOME/Qt/latest/$KIT_DIR\""
assert_contains "$OUT" "set \"IPPONBOARD_ROOT_DIR=$REPO_DIR\""

# --- home fails when no version directory exists
EMPTY_HOME="$WORK_DIR/empty-home"
mkdir -p "$EMPTY_HOME/Qt"
assert_fails "profile home fails without Qt versions" env HOME="$EMPTY_HOME" "$GENERATOR" home "$OUT"

# --- ud prompts for QTDIR and IPPONBOARD_ROOT_DIR
UD_QT="$WORK_DIR/ud-qt"
UD_ROOT="$WORK_DIR/ud-root"
mkdir -p "$UD_QT" "$UD_ROOT"
assert_succeeds "profile ud succeeds" \
    bash -c "printf '%s\n%s\n' '$UD_QT' '$UD_ROOT' | '$GENERATOR' ud '$OUT'"
assert_contains "$OUT" "set \"$QTDIR_VAR=$UD_QT\""
assert_contains "$OUT" "set \"IPPONBOARD_ROOT_DIR=$UD_ROOT\""
assert_cfg_format "$OUT"

# --- ud with empty root falls back to the output file directory
assert_succeeds "profile ud with default root succeeds" \
    bash -c "printf '%s\n\n' '$UD_QT' | '$GENERATOR' ud '$OUT'"
assert_contains "$OUT" "set \"IPPONBOARD_ROOT_DIR=$REPO_DIR\""

# --- ud rejects a missing QTDIR
assert_fails "profile ud fails on missing QTDIR" \
    bash -c "printf '%s\n%s\n' '$WORK_DIR/no-such-dir' '$UD_ROOT' | '$GENERATOR' ud '$OUT'"

# --- QTDIR suggestions depend on the package manager in use
STUB_BIN="$WORK_DIR/stub-bin"
BREW_QT="$WORK_DIR/brew-qt"
mkdir -p "$STUB_BIN" "$BREW_QT"

function pkg_manager_suggestion {
    env PATH="$STUB_BIN" "$(command -v bash)" -c "source '$GENERATOR'; detect_default_qtdir ${1:-}" 2> /dev/null
}

# --- explicit package manager selection
if [ "$(pkg_manager_suggestion dnf)" = "/usr/lib64/qt6" ]; then
    report 0 "dnf selection suggests /usr/lib64/qt6"
else
    report 1 "dnf selection suggests /usr/lib64/qt6"
fi
if [ "$(pkg_manager_suggestion apt)" = "/usr/lib/qt6" ]; then
    report 0 "apt selection suggests /usr/lib/qt6"
else
    report 1 "apt selection suggests /usr/lib/qt6"
fi

# --- auto detection
touch "$STUB_BIN/dnf" "$STUB_BIN/apt"
chmod +x "$STUB_BIN/dnf" "$STUB_BIN/apt"
if [ "$(pkg_manager_suggestion)" = "/usr/lib64/qt6" ]; then
    report 0 "dnf suggests /usr/lib64/qt6 (and wins over apt)"
else
    report 1 "dnf suggests /usr/lib64/qt6 (and wins over apt)"
fi

rm -f "$STUB_BIN/dnf"
if [ "$(pkg_manager_suggestion)" = "/usr/lib/qt6" ]; then
    report 0 "apt suggests /usr/lib/qt6"
else
    report 1 "apt suggests /usr/lib/qt6"
fi

rm -f "$STUB_BIN/apt"
cat > "$STUB_BIN/brew" <<EOF_BREW
#!/bin/bash
echo "$BREW_QT"
EOF_BREW
chmod +x "$STUB_BIN/brew"
if [ "$(pkg_manager_suggestion)" = "$BREW_QT" ]; then
    report 0 "brew suggests its Qt prefix"
else
    report 1 "brew suggests its Qt prefix"
fi

rm -f "$STUB_BIN/brew"
assert_fails "no package manager gives no suggestion" pkg_manager_suggestion

# --- ud falls back to the suggested QTDIR when the input is empty
cat > "$STUB_BIN/brew" <<EOF_BREW
#!/bin/bash
echo "$BREW_QT"
EOF_BREW
cat > "$STUB_BIN/uname" <<'EOF_UNAME'
#!/bin/bash
echo Linux
EOF_UNAME
chmod +x "$STUB_BIN/brew" "$STUB_BIN/uname"
ln -sf "$(command -v dirname)" "$STUB_BIN/dirname"
ln -sf "$(command -v cat)" "$STUB_BIN/cat"
rm -f "$OUT"
assert_succeeds "profile ud with suggested QTDIR succeeds" \
    bash -c "printf '\n\n' | env PATH='$STUB_BIN' '$GENERATOR' ud '$OUT'"
assert_contains "$OUT" "set \"LINUX_QTDIR=$BREW_QT\""
assert_contains "$OUT" "set \"IPPONBOARD_ROOT_DIR=$REPO_DIR\""

# --- profile brew uses the brew prefix
assert_succeeds "profile brew uses the brew prefix" \
    env PATH="$STUB_BIN" "$GENERATOR" brew "$OUT"
assert_contains "$OUT" "set \"LINUX_QTDIR=$BREW_QT\""

# --- a profile overwrites an existing configuration
assert_succeeds "overwrite succeeds" "$GENERATOR" apt "$OUT"
assert_contains "$OUT" "set \"$QTDIR_VAR=/usr/lib/qt6\""
assert_not_contains "$OUT" "$UD_QT"

# --- unknown profiles fail without touching the configuration
assert_fails "unknown profile fails" "$GENERATOR" bogus "$OUT"
assert_contains "$OUT" "set \"$QTDIR_VAR=/usr/lib/qt6\""

# --- default output file is env_cfg.bat in the working directory
rm -f "$OUT"
assert_succeeds "default output path succeeds" bash -c "cd '$REPO_DIR' && '$GENERATOR' apt"
assert_contains "$OUT" "set \"$QTDIR_VAR=/usr/lib/qt6\""

echo
echo "$TESTS tests, $FAILURES failures"
if [ "$FAILURES" -ne 0 ]; then
    exit 1
fi
exit 0
