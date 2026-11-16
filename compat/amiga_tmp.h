/* amiga_tmp.h - see amiga_tmp.c.  Copyright (c) 2026 AmiCradle Team. LGPL 2.1+ */
#ifndef AMIGA_TMP_H
#define AMIGA_TMP_H
#include <stdio.h>
#include <stddef.h>
void amiga_join(char *out, size_t size, const char *dir, const char *name);
FILE *amiga_tmpfile(const char *dir);
#endif
