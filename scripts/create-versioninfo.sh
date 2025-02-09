#!/bin/bash
#---------------------------------------------------------
# Copyright 2024 Florian Muecke. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE.txt file.
#---------------------------------------------------------

set -e
BASE_DIR="$1"

echo "Creating version information..."
if [ -z "$BASE_DIR" ]; then
  echo "Argument BASE_DIR missing for $0"
  exit 1
fi
echo "Using: BASE_DIR=$BASE_DIR"

# --> CHANGE VERSION HERE:
VER1=2
VER2=4
VER3=0
TAG=""
# that's it. <--

REV=$(git log -1 --format=%h)
echo "$REV" > "$BASE_DIR/.revision"

REV_DATE=$(git log -1 --format=%ci)
echo "$REV_DATE" > "$BASE_DIR/.date"

FILENAME_NO_EXT="$BASE_DIR/versioninfo"
BUILDNR_FILE="$BASE_DIR/.buildnr"
if [ ! -f "$BUILDNR_FILE" ]; then
  VER4=0
else
  # read just the first line without line endings
  VER4=$(head -n 1 "$BUILDNR_FILE" | tr -d '\r')
  if [ ! -f "$FILENAME_NO_EXT.h" ]; then
    VER4=$((VER4 + 1))
  fi
fi
echo "$VER4" > "$BASE_DIR/.buildnr"

if [ -f "$FILENAME_NO_EXT.h" ]; then
  echo "--> version info up to date: $VER1.$VER2.$VER3.$VER4"
  exit 0
fi

IPPONBOARD_VERSION="$VER1.$VER2.$VER3.$VER4"

# write template
echo "-> writing version info template: v=$IPPONBOARD_VERSION"
{
  echo "//"
  echo "// THIS FILE IS GENERATED - DO NOT MODIFY!"
  echo "//"
  echo "#ifndef BASE__VERSIONINFO_H_"
  echo "#define BASE__VERSIONINFO_H_"
  echo "namespace VersionInfo"
  echo "{"
  echo "  const char* const Revision = \"$REV\";"
  echo "  const char* const Date = \"$REV_DATE\";"
  echo "  const char* const CopyrightYear = \"${REV_DATE:0:4}\";"
  echo "  const char* const VersionStrShort = \"$VER1.$VER2.$VER3\";"
  echo "  const char* const VersionStrFull = \"$VER1.$VER2.$VER3.$VER4\";"
  if [ -z "$TAG" ]; then
    echo "  const char* const VersionStr = \"$VER1.$VER2.$VER3\";"
  else
    echo "  const char* const VersionStr = \"$VER1.$VER2.$VER3-$TAG\";"
  fi
  echo "}"
  echo "#endif  // BASE__VERSIONINFO_H_"
} > "$FILENAME_NO_EXT.tmp"

echo "-> generating versioninfo.h"
# generate header file
mv "$FILENAME_NO_EXT.tmp" "$FILENAME_NO_EXT.h"

# --
# Update RC file
# --
echo "-> generating RC file"
RC_FILE="$BASE_DIR/Ipponboard.rc"
{
  echo "//"
  echo "// FILE IS GENERATED - DO NOT CHANGE!!"
  echo "//"
  echo "#include <winver.h>"
  echo "#pragma code_page(1252)"
  echo "101 ICON \"images/ipponboard.ico\""
  echo "/////////////////////////////////////////////////////////////////////////////"
  echo "VS_VERSION_INFO VERSIONINFO"
  echo "FILEVERSION $VER1,$VER2,$VER3,$VER4"
  echo "PRODUCTVERSION $VER1,$VER2,$VER3,$VER4"
  echo "FILEFLAGSMASK 0x3fL"
  echo "#ifdef _DEBUG"
  echo "FILEFLAGS 0x1L"
  echo "#else"
  echo "FILEFLAGS 0x0L"
  echo "#endif"
  echo "FILEOS 0x40004L"
  echo "FILETYPE 0x1L"
  echo "FILESUBTYPE 0x0L"
  echo "BEGIN"
  echo "    BLOCK \"StringFileInfo\""
  echo "    BEGIN"
  echo "        BLOCK \"040004b0\""
  echo "        BEGIN"
  echo "            VALUE \"CompanyName\", \"Florian Muecke\""
  echo "            VALUE \"FileDescription\", \"Ipponboard\""
  echo "            VALUE \"FileVersion\", \"$VER1.$VER2.$VER3.$VER4\""
  echo "            VALUE \"InternalName\", \"Ipponboard.exe\""
  echo "            VALUE \"LegalCopyright\", \"Copyright (C) 2010-${REV_DATE:0:4} Florian Muecke\""
  echo "            VALUE \"OriginalFilename\", \"Ipponboard.exe\""
  echo "            VALUE \"ProductName\", \"Ipponboard\""
  if [ -z "$TAG" ]; then
    echo "            VALUE \"ProductVersion\", \"$VER1.$VER2.$VER3\""
  else
    echo "            VALUE \"ProductVersion\", \"$VER1.$VER2.$VER3-$TAG\""
  fi
  echo "        END"
  echo "    END"
  echo "    BLOCK \"VarFileInfo\""
  echo "    BEGIN"
  echo "        VALUE \"Translation\", 0x400, 1200"
  echo "    END"
  echo "END"
  echo "/////////////////////////////////////////////////////////////////////////////"
} > "$RC_FILE"
