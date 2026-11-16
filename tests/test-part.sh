#!/bin/sh
# Host test for scripts/write-part.sh: the Part file GFOpenUpTool reads.
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
T=$(mktemp -d); trap 'rm -rf "$T"' EXIT
mkdir -p "$T/C" "$T/Docs/SoX"
: > "$T/C/sox"; : > "$T/Docs/SoX/LICENSE.GPL"
sh "$HERE/../scripts/write-part.sh" "$T" 14.4.3
fail=0
expect() { grep -qxF "$1" "$T/Part" || { echo "FAIL: no line: $1"; fail=1; }; }
expect 'part SoX 14.4.3'
expect 'needs 68040'
expect 'copy "C/sox" "SYS:C/sox"'
expect 'copy "Docs/SoX/LICENSE.GPL" "SYS:Docs/SoX/LICENSE.GPL"'
grep -q 'needs part' "$T/Part" && { echo "FAIL: sox needs no other part (AHI is the system's)"; fail=1; }
[ "$(grep -c '^copy' "$T/Part")" = 2 ] || { echo "FAIL: copy count (the Part file itself must not be listed)"; fail=1; }
sh "$HERE/../scripts/write-part.sh" "$T" 14.4.3
[ "$(grep -c '^copy' "$T/Part")" = 2 ] || { echo "FAIL: second run lists the Part file"; fail=1; }
[ $fail = 0 ] && echo "Part file: all checks passed"
exit $fail
