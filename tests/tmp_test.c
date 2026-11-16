/* Host test for compat/amiga_tmp.c: AmigaDOS joins and temporary files
 * deleted at exit.  Copyright (c) 2026 AmiCradle Team. LGPL 2.1 or later. */
#include <stdio.h>
#include <string.h>
#include "amiga_tmp.h"

static int fails;
#define CHECK(c) do { if (!(c)) { printf("FAIL %s:%d %s\n", __FILE__, __LINE__, #c); fails++; } } while (0)

int main(void)
{
    char b[64];
    amiga_join(b, sizeof b, "T:", "x"); CHECK(!strcmp(b, "T:x"));
    amiga_join(b, sizeof b, NULL, "x"); CHECK(!strcmp(b, "T:x"));
    amiga_join(b, sizeof b, "", "x"); CHECK(!strcmp(b, "T:x"));
    amiga_join(b, sizeof b, "Work:Temp", "x"); CHECK(!strcmp(b, "Work:Temp/x"));
    amiga_join(b, sizeof b, "Work:Temp/", "x"); CHECK(!strcmp(b, "Work:Temp/x"));
    amiga_join(b, sizeof b, "RAM:", "x"); CHECK(!strcmp(b, "RAM:x"));
    {
        FILE *f = amiga_tmpfile("tmpdir"), *g = amiga_tmpfile("tmpdir");
        CHECK(f && g && f != g);
        if (f) { fputs("abc", f); rewind(f); CHECK(fgetc(f) == 'a'); fclose(f); }
        if (g) fclose(g);
    }
    printf("tmp_test: %s\n", fails ? "FAIL" : "8/8 pass (files go at exit)");
    return fails != 0;
}
