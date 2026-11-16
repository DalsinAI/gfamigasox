/* amiga_tmp.c - libSoX's temporary files on the Amiga (lsx_tmpfile, for
 * 8SVX writing, `gain -n', `reverse' and the like).  SoX's own way, mkstemp
 * then unlink while open, leaves the file behind on AmigaDOS (an open file
 * cannot be deleted) and joins "T:" + "/name" into the parent of T:.  Here
 * the name is joined the AmigaDOS way, the file is opened "w+b" and its name
 * kept, and each is deleted at exit, once SoX has closed it.
 * Copyright (c) 2026 AmiCradle Team. LGPL 2.1 or later, like libsox. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amiga_tmp.h"

/* dir + name: no '/' after a device or a drawer that already ends one */
void amiga_join(char *out, size_t size, const char *dir, const char *name)
{
    size_t n;
    if (!dir || !*dir)
        dir = "T:";
    n = strlen(dir);
    snprintf(out, size, "%s%s%s", dir,
             (dir[n - 1] == ':' || dir[n - 1] == '/') ? "" : "/", name);
}

static struct tmp { struct tmp *next; char name[256]; } *tmps;
static unsigned long count;

static void amiga_tmp_cleanup(void)
{
    while (tmps) {
        struct tmp *t = tmps;
        tmps = t->next;
        remove(t->name);    /* its user has closed it (SoX always does) */
        free(t);
    }
}

FILE *amiga_tmpfile(const char *dir)
{
    struct tmp *t = malloc(sizeof(*t));
    char leaf[48];
    if (!t)
        return NULL;
    if (!tmps && !count)
        atexit(amiga_tmp_cleanup);
    snprintf(leaf, sizeof leaf, "libSoX.%lx.%lu", (unsigned long)(size_t)&count, ++count);
    amiga_join(t->name, sizeof t->name, dir, leaf);
    FILE *f = fopen(t->name, "w+b");
    if (!f) {
        free(t);
        return NULL;
    }
    t->next = tmps;
    tmps = t;
    return f;
}
