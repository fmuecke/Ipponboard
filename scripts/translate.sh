#!/bin/bash

# Updates and compiles the translations (counterpart of scripts/translate.cmd).
# Runs lupdate, opens linguist for reviewing and lrelease to build the .qm files.
#
# Usage: translate.sh
# Requires QTDIR and IPPONBOARD_ROOT_DIR in the environment (see env_cfg.bat).

set -u

if [ -z "${QTDIR:-}" ]; then
    echo "ERROR: QTDIR not set." >&2
    exit 1
fi
if [ -z "${IPPONBOARD_ROOT_DIR:-}" ]; then
    echo "ERROR: IPPONBOARD_ROOT_DIR not set." >&2
    exit 1
fi

# Resolves the Qt tool directory below QTDIR: either $QTDIR/bin or the kit
# directory of a Qt installer tree (e.g. $QTDIR/gcc_64/bin).
function resolve_qt_bin_dir {
    if [ -x "$QTDIR/bin/lupdate" ]; then
        echo "$QTDIR/bin"
        return 0
    fi

    local kit
    for kit in gcc_64 clang_64 macos; do
        if [ -x "$QTDIR/$kit/bin/lupdate" ]; then
            echo "$QTDIR/$kit/bin"
            return 0
        fi
    done

    return 1
}

QT_BIN_DIR=$(resolve_qt_bin_dir) || {
    echo "ERROR: lupdate not found in $QTDIR/bin or $QTDIR/<kit>/bin. QTDIR must point at the Qt installation configured in env_cfg.bat." >&2
    exit 1
}

LUPDATE="$QT_BIN_DIR/lupdate"
LINGUIST="$QT_BIN_DIR/linguist"
LRELEASE="$QT_BIN_DIR/lrelease"

for tool in "$LINGUIST" "$LRELEASE"; do
    if [ ! -x "$tool" ]; then
        echo "ERROR: $tool not found in $QT_BIN_DIR. It must belong to the Qt installation configured in env_cfg.bat." >&2
        exit 1
    fi
done

# -sort-messages is not supported by older lupdate versions; use it only if available
SORT_OPTION=""
if "$LUPDATE" -help 2>&1 | grep -q "sort-messages"; then
    SORT_OPTION="-sort-messages"
fi

"$LUPDATE" -no-obsolete -locations none -no-recursive $SORT_OPTION \
    "$IPPONBOARD_ROOT_DIR/base" "$IPPONBOARD_ROOT_DIR/core" "$IPPONBOARD_ROOT_DIR/Widgets" \
    -ts "$IPPONBOARD_ROOT_DIR/i18n/de.ts" -ts "$IPPONBOARD_ROOT_DIR/i18n/nl.ts" || exit 1

read -p "Press enter to continue"

"$LINGUIST" "$IPPONBOARD_ROOT_DIR/i18n/de.ts" "$IPPONBOARD_ROOT_DIR/i18n/nl.ts" || exit 1

"$LRELEASE" -compress "$IPPONBOARD_ROOT_DIR/i18n/de.ts" -qm "$IPPONBOARD_ROOT_DIR/i18n/de.qm" || exit 1
"$LRELEASE" -compress "$IPPONBOARD_ROOT_DIR/i18n/nl.ts" -qm "$IPPONBOARD_ROOT_DIR/i18n/nl.qm" || exit 1
exit 0
