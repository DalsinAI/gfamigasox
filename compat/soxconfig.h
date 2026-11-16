/* soxconfig.h for SoX 14.4.3git on GFOS (AmigaOS 3.2.3, 68040, libnix):
 * what SoX's configure would find there.  Every format is built in (no
 * libltdl plugins); the formats that need outside libraries (FLAC, Ogg,
 * MP3, PNG spectrograms...) are left out.  Host tests build with
 * GFAMIGA_HOST_TEST, which only flips the byte order to the host's.
 * Copyright (c) 2026 AmiCradle Team. GPL 2 or later, like SoX. */
#ifndef GFAMIGA_SOXCONFIG_H
#define GFAMIGA_SOXCONFIG_H
#define PACKAGE_NAME "SoX"
#define PACKAGE_VERSION "14.4.3git"
#define PACKAGE_STRING "SoX 14.4.3git"
#define PACKAGE_EXTRA "GFAmigaSoX"
#ifndef GFAMIGA_HOST_TEST
#define WORDS_BIGENDIAN 1
#endif
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRCASECMP 1
#define HAVE_VSNPRINTF 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_LRINT 1
#define HAVE_MKSTEMP 1
/* not here: popen (no pipes in or out), termios (no interactive keys), glob,
 * fseeko (files up to 2 GB), sigaction, fmemopen, OpenMP */
#if defined __amigaos__ && !defined GFAMIGA_HOST_TEST
/* our own output: ahi.c, -t ahi and play, through ahi.device */
#define HAVE_AHI 1
#define STATIC_AHI 1
#endif
#endif
