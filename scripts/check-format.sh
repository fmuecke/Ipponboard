#!/bin/bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
CLANG_FORMAT=${CLANG_FORMAT:-clang-format}
REQUIRED_VERSION_MAJOR=14
REQUIRED_VERSION_MINOR=0

if ! command -v "$CLANG_FORMAT" >/dev/null 2>&1; then
    echo "ERROR: clang-format not found. Install LLVM clang-format ${REQUIRED_VERSION_MAJOR}.x and ensure it is on PATH." >&2
    exit 1
fi

VERSION_STR=$($CLANG_FORMAT --version)
if [[ $VERSION_STR =~ ([0-9]+)\.([0-9]+)\. ]]; then
    MAJOR=${BASH_REMATCH[1]}
    MINOR=${BASH_REMATCH[2]}
    if (( MAJOR < REQUIRED_VERSION_MAJOR || (MAJOR == REQUIRED_VERSION_MAJOR && MINOR < REQUIRED_VERSION_MINOR) )); then
        echo "ERROR: clang-format version $MAJOR.$MINOR detected. Require >= ${REQUIRED_VERSION_MAJOR}.${REQUIRED_VERSION_MINOR}." >&2
        exit 1
    fi
else
    echo "WARNING: Unable to parse clang-format version string '$VERSION_STR'. Proceeding." >&2
fi

cd "$ROOT_DIR"

MODIFIED=$( { git diff --name-only --diff-filter=ACMRTUXB HEAD || true; git diff --name-only --cached --diff-filter=ACMRTUXB; } | sort -u )
FILES=()
for file in $MODIFIED; do
    case "$file" in
        *.c|*.cc|*.cpp|*.cxx|*.h|*.hh|*.hpp)
            if [[ -f "$file" ]]; then
                FILES+=("$file")
            fi
            ;;
    esac
done

if [[ ${#FILES[@]} -eq 0 ]]; then
    echo "No modified C/C++ files detected for formatting check."
    exit 0
fi

FAILED=0
for file in "${FILES[@]}"; do
    if ! "$CLANG_FORMAT" --dry-run --Werror "$file" >/dev/null 2>&1; then
        echo "Formatting issues found in $file" >&2
        FAILED=1
    fi

done

if [[ $FAILED -ne 0 ]]; then
    echo "clang-format check failed. Run clang-format on the reported files." >&2
    exit 1
fi

echo "clang-format check passed."
