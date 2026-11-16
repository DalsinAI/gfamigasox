# The SoX sources sox is built from: SoX's own src/Makefile.am lists, every
# format built in, without the ones that need outside libraries.  Sourced by
# build-amiga.sh and tests/run-host-tests.sh.
# Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX.
LIBSOX="adpcms aiff cvsd g711 g721 g723_24 g723_40 g72x vox raw formats formats_i skelform
 xmalloc getopt util libsox libsox_i sox-fmt
 bend biquad biquads chorus compand compandt contrast dcshift delay dft_filter dither divide
 downsample earwax echo echos effects effects_i effects_i_dsp fade fft4g fir firfit flanger gain
 hilbert input ladspa loudness mcompand noiseprof noisered output overdrive pad phaser rate
 remix repeat reverb reverse silence sinc skeleff speed splice stat stats stretch swap
 synth tempo tremolo trim upsample vad vol
 raw-fmt s1-fmt s2-fmt s3-fmt s4-fmt u1-fmt u2-fmt u3-fmt u4-fmt al-fmt la-fmt ul-fmt
 lu-fmt 8svx aiff-fmt aifc-fmt au avr cdr cvsd-fmt dvms-fmt dat hcom htk maud prc sf smp
 sounder soundtool sphere tx16w voc vox-fmt ima-fmt adpcm ima_rw wav wve xa nulfile f4-fmt
 f8-fmt gsrt id3"
