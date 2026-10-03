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

LUPDATE="$QTDIR/bin/lupdate"
LINGUIST="$QTDIR/bin/linguist"
LRELEASE="$QTDIR/bin/lrelease"

for tool in "$LUPDATE" "$LINGUIST" "$LRELEASE"; do
    if [ ! -x "$tool" ]; then
        echo "ERROR: $tool not found. It must belong to the Qt installation configured in env_cfg.bat." >&2
        exit 1
    fi
done

"$LUPDATE" -no-obsolete -locations none -no-recursive -sort-messages \
    "$IPPONBOARD_ROOT_DIR/base" "$IPPONBOARD_ROOT_DIR/core" "$IPPONBOARD_ROOT_DIR/Widgets" \
    -ts "$IPPONBOARD_ROOT_DIR/i18n/de.ts" -ts "$IPPONBOARD_ROOT_DIR/i18n/nl.ts" || exit 1

read -p "Press enter to continue"

"$LINGUIST" "$IPPONBOARD_ROOT_DIR/i18n/de.ts" "$IPPONBOARD_ROOT_DIR/i18n/nl.ts" || exit 1

"$LRELEASE" -compress "$IPPONBOARD_ROOT_DIR/i18n/de.ts" -qm "$IPPONBOARD_ROOT_DIR/i18n/de.qm" || exit 1
"$LRELEASE" -compress "$IPPONBOARD_ROOT_DIR/i18n/nl.ts" -qm "$IPPONBOARD_ROOT_DIR/i18n/nl.qm" || exit 1
exit 0
