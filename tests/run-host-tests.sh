#!/bin/sh
# Host tests: SoX with patches/ and compat/ (soxconfig.h with the host's byte
# order, GFAMIGA_HOST_TEST; -D__amigaos__ so libSoX's temporary files go
# through compat/amiga_tmp.c as on the Amiga; no AHI on the host), then
#  - amiga_tmp: AmigaDOS joins, temporary files deleted at exit;
#  - 8-bit mono WAV -> 8SVX -> WAV and 16-bit stereo WAV -> AIFF -> WAV, bit
#    for bit; 16-bit stereo -> 8SVX (its temporary files) has the right length;
#  - effects: reverse twice gives the input back (temporary files again),
#    rate 22050 halves the length, vol 0.5 halves the peak;
#  - no temporary file is left behind.
#   sh tests/run-host-tests.sh      (WORK=... to keep the checkout elsewhere)
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX.
set -eu
HERE=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
. "$HERE/sox.lock"; . "$HERE/scripts/sources.sh"
WORK=${WORK:-$(mktemp -d)}
SRC="$WORK/sox"
if [ "$(git -C "$SRC" rev-parse HEAD 2>/dev/null)" != "$SOX_COMMIT" ]; then
    rm -rf "$SRC"; mkdir -p "$SRC"; git -C "$SRC" init -q
    git -C "$SRC" fetch -q --depth 1 "$SOX_GIT" "$SOX_COMMIT"; git -C "$SRC" checkout -q FETCH_HEAD
    for p in "$HERE"/patches/*.patch; do patch -s -d "$SRC" -p1 < "$p"; done
fi
O="$WORK/host"; rm -rf "$O"; mkdir -p "$O/tmpdir"
CF="-O1 -w -DGFAMIGA_HOST_TEST -D__amigaos__ -I$HERE/compat -I$SRC/src"
objs=""
for f in $LIBSOX sox; do cc $CF -c "$SRC/src/$f.c" -o "$O/$f.o"; objs="$objs $O/$f.o"; done
cc $CF -c "$HERE/compat/amiga_tmp.c" -o "$O/amiga_tmp.o"
cc -o "$O/sox" $objs "$O/amiga_tmp.o" -lm
cc $CF -o "$O/tmp_test" "$HERE/tests/tmp_test.c" "$O/amiga_tmp.o"
fail=0
(cd "$O" && ./tmp_test) || fail=1
[ -z "$(ls "$O/tmpdir")" ] || { echo "FAIL: tmp_test left $(ls "$O/tmpdir")"; fail=1; }
python3 "$HERE/tests/make_audio.py" "$O"
cd "$O"
S=./sox
$S t16.wav -b 8 -c 1 u8.wav remix 1 2>/dev/null
$S u8.wav u8.8svx; $S u8.8svx u8.back.wav
cmp -s u8.wav u8.back.wav || { echo "FAIL: 8-bit WAV -> 8SVX -> WAV differs"; fail=1; }
$S t16.wav t16.aiff; $S t16.aiff t16.back.wav
cmp -s t16.wav t16.back.wav || { echo "FAIL: 16-bit WAV -> AIFF -> WAV differs"; fail=1; }
$S t16.wav st.8svx 2>/dev/null
[ "$($S --i -s st.8svx)" = 66150 ] || { echo "FAIL: stereo 8SVX length $($S --i -s st.8svx)"; fail=1; }
$S t16.wav r1.wav reverse; $S r1.wav r2.wav reverse
cmp -s t16.wav r2.wav || { echo "FAIL: reverse twice differs"; fail=1; }
$S t16.wav h.wav rate 22050
n=$($S --i -s h.wav); [ "$n" -ge 33074 ] && [ "$n" -le 33076 ] || { echo "FAIL: rate 22050 gives $n samples"; fail=1; }
p1=$($S t16.wav -n stat 2>&1 | awk '/Maximum amplitude/ {print $3}')
p2=$($S t16.wav -n vol 0.5 stat 2>&1 | awk '/Maximum amplitude/ {print $3}')
awk -v a="$p1" -v b="$p2" 'BEGIN { exit !(b > a/2 - 0.001 && b < a/2 + 0.001) }' || { echo "FAIL: vol 0.5: $p1 -> $p2"; fail=1; }
left=$(ls | grep '^T:libSoX' || true)
[ -z "$left" ] || { echo "FAIL: temporary files left: $left"; fail=1; }
[ $fail = 0 ] && echo "sox on the host: all checks passed"
exit $fail
