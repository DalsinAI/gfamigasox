# Writes the test sounds tests/run-host-tests.sh sends through sox: t16.wav
# (16-bit stereo, 44.1 kHz, 1.5 s) and t8.aiff (8-bit mono, 8363 Hz, 8364 samples, as an
# Amiga sample would be).  Deterministic: no randomness.
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX.
import math, os, struct, sys
d = sys.argv[1]
n = 66150
pcm = bytearray()
for i in range(n):
    l = int(12000 * math.sin(2 * math.pi * 440 * i / 44100) + 3000 * math.sin(i * 0.37))
    r = int(9000 * math.sin(2 * math.pi * 523.25 * i / 44100) + ((i * 7919) % 401) - 200)
    pcm += struct.pack('<hh', l, r)
with open(os.path.join(d, 't16.wav'), 'wb') as f:
    f.write(b'RIFF' + struct.pack('<I', 36 + len(pcm)) + b'WAVE')
    f.write(b'fmt ' + struct.pack('<IHHIIHH', 16, 1, 2, 44100, 44100 * 4, 4, 16))
    f.write(b'data' + struct.pack('<I', len(pcm)) + pcm)

def ext80(x):  # IEEE 754 80-bit extended, for the AIFF sample rate
    e = int(math.floor(math.log2(x)))
    m = int(x / 2 ** e * (1 << 63))
    return struct.pack('>HQ', e + 16383, m)
m = 8364  # even: an AIFF chunk of odd length needs a pad byte
s8 = bytes((int(100 * math.sin(i / 9.0)) + ((i * 31) % 7) - 3) & 0xff for i in range(m))
comm = struct.pack('>hIh', 1, m, 8) + ext80(8363)
ssnd = struct.pack('>II', 0, 0) + s8
body = b'AIFF' + b'COMM' + struct.pack('>I', len(comm)) + comm + b'SSND' + struct.pack('>I', len(ssnd)) + ssnd
with open(os.path.join(d, 't8.aiff'), 'wb') as f:
    f.write(b'FORM' + struct.pack('>I', len(body)) + body)
