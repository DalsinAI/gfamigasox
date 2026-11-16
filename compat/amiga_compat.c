/* GFAmigaSoX: the stack libnix's start-up sets up (a Shell gives 4 KB; the
 * rate and FFT effects want more) and the version string.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX. */
unsigned long __stack = 256 * 1024;
static const char ver[] __attribute__((used)) = "$VER: sox 14.4.3 (16.11.2026) GFAmigaSoX";
