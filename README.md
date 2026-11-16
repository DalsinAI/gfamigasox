# GFAmigaSoX

SoX, the "Swiss Army knife of sound processing", for GFOS (AmigaOS 3.2.3
with GFOpenUp) on a 68040: one `sox` command that converts between sound
formats, runs some 60 effects (rate, vol, reverse, trim, fade, reverb, echo,
compand, noise reduction, pitch, tempo, stats...) and plays the result
through AHI. It comes in the GFOpenUp part **SoX**:

    GFOpenUpTool INSTALL Parts/SoX

    sox Work:Samples/loop.wav Work:Samples/loop.8svx         ; to 8SVX
    sox Work:Samples/drums.aiff -r 22050 -b 8 RAM:drums.8svx  ; for Paula
    sox Work:Samples/voice.wav -d                            ; play it
    sox Work:Samples/voice.wav -d reverb 50 tempo 1.2        ; play, changed
    sox Work:Samples/voice.wav -n stat                       ; measure it

Formats built in: WAV, AIFF/AIFC, 8SVX, MAUD, AU/SND, VOC, AVR, SMP, HCOM,
Sounder, SoundTool, Sphere, TX16W, PRC, SF (IRCAM), CDR, DAT, raw PCM, A-law,
mu-law, IMA and MS ADPCM, CVSD, VOX and more (`sox --help` lists them). Not
built: MP3, Ogg Vorbis, FLAC, Opus (each needs its own library; FLAC has its
own part, GFAmigaFLAC) and the PNG spectrogram.

## Playing: -d and -t ahi

`-d` is AHI: `compat/ahi.c` is SoX's output to `ahi.device`, 16-bit mono or
stereo, double-buffered so there is no gap. The AHI unit is 0 (set by the AHI
preferences); `-t ahi 1` plays on unit 1, or set `AUDIODEV`. Ctrl-C stops.
More than two channels: add `remix 1 2` or `channels 2`.

## Temporary files

Some effects and 8SVX stereo writing use temporary files: they go in `T:`
(or `--temp <drawer>`) and are deleted when sox ends.

## Building

    STOVE=~/AmiCradle/kitchen/stoves/os32-gcc16-open6 OUT=out ./build-amiga.sh

SoX (chirlu/sox master: 14.4.2 with the security fixes since) is fetched at
the commit `sox.lock` pins and patched from `patches/` (the AHI format in the
static list and first as the default device; temporary files through
`compat/amiga_tmp.c`; piped input needs `-t`, as libnix's FILE cannot be wound back); `compat/soxconfig.h` stands for SoX's configure. It
builds with soft float, taking the FPU where there is one (fpuauto).

## Tests

    make -C tests test

builds patched SoX for the host and checks 8-bit WAV to 8SVX and back and
16-bit WAV to AIFF and back bit for bit, stereo 8SVX (temporary files),
`reverse` twice, `rate` and `vol`, AmigaDOS joins for `T:`, and that no
temporary file is left. AHI playing is checked on an Amiga (no AHI on a host).

Designed, tested and delivered by The AmiCradle Team.
SoX is Copyright (c) Chris Bagwell, Lance Norskog and the SoX contributors:
sox under the GPL 2 or later, libSoX under the LGPL 2.1 or later.
