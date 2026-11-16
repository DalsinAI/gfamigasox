#!/bin/sh
# Writes the Part file GFOpenUpTool INSTALL reads (gfopenup src/openuptool.c):
# one "copy" line per file under the part drawer, each to SYS:<its path>.
#   write-part.sh <part drawer> <version>
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX.
set -eu
P=$1; V=$2
{
    echo "part SoX $V"
    echo "; SoX: convert, process and play sounds (WAV, AIFF, 8SVX, MAUD, AU, VOC and more; 60 effects); plays through AHI"
    echo "needs 68040"
    (cd "$P" && find C Docs -type f ! -name Part 2>/dev/null | LC_ALL=C sort) | while read -r f; do
        echo "copy \"$f\" \"SYS:$f\""
    done
} > "$P/Part"
