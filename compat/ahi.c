/* ahi.c - SoX's audio output on the Amiga: "-t ahi" and "-d" (the default
 * device), through ahi.device, so sox plays a sound as it processes it:
 *   sox Work:Samples/loop.wav -d
 *   sox Work:Samples/loop.8svx -t ahi 1 rate 22050   (unit 1)
 * The "file name" is the AHI unit (0-3, default 0, or AUDIODEV).  16-bit
 * signed samples, mono or stereo, double-buffered: one request plays while
 * the next is filled, linked with ahir_Link so there is no gap.  Ctrl-C stops.
 * Copyright (c) 2026 AmiCradle Team. LGPL 2.1 or later, like libsox. */
#include "sox_i.h"
#include <stdlib.h>
#include <string.h>
#include <exec/exec.h>
#include <devices/ahi.h>
#include <dos/dos.h>
#include <proto/exec.h>

#define FRAMES 8192 /* per buffer: 0.19 s at 44.1 kHz */

typedef struct {
    struct MsgPort *port;
    struct AHIRequest *req[2];
    struct AHIRequest *link;   /* the request playing now, or NULL */
    int busy[2];               /* sent and not yet taken back with WaitIO */
    int16_t *buf[2];
    size_t fill, size;         /* samples in buf[cur], samples a buffer holds */
    int cur, open;
    ULONG type;
} priv_t;

static void ahi_close(priv_t *p)
{
    for (int i = 0; i < 2; i++)
        if (p->busy[i]) {
            if (!CheckIO((struct IORequest *)p->req[i]))
                AbortIO((struct IORequest *)p->req[i]);
            WaitIO((struct IORequest *)p->req[i]);
            p->busy[i] = 0;
        }
    p->link = NULL;
    if (p->open)
        CloseDevice((struct IORequest *)p->req[0]);
    p->open = 0;
    if (p->req[1])
        FreeVec(p->req[1]);
    if (p->req[0])
        DeleteIORequest((struct IORequest *)p->req[0]);
    if (p->port)
        DeleteMsgPort(p->port);
    free(p->buf[0]);
    free(p->buf[1]);
    memset(p, 0, sizeof(*p));
}

static int startwrite(sox_format_t *ft)
{
    priv_t *p = (priv_t *)ft->priv;
    char const *name = ft->filename;
    ULONG unit = AHI_DEFAULT_UNIT;

    if (name && *name >= '0' && *name <= '3' && !name[1])
        unit = (ULONG)(*name - '0');
    if (ft->signal.channels > 2) {
        lsx_fail_errno(ft, SOX_EOF, "AHI plays mono or stereo: add `remix 1 2' or `channels 2'");
        return SOX_EOF;
    }
    if (!ft->signal.channels)
        ft->signal.channels = 2;
    if (!ft->signal.rate)
        ft->signal.rate = 44100;
    ft->encoding.encoding = SOX_ENCODING_SIGN2;
    ft->encoding.bits_per_sample = 16;
    ft->signal.precision = 16;
    p->type = ft->signal.channels == 2 ? AHIST_S16S : AHIST_M16S;
    p->size = FRAMES * ft->signal.channels;

    if (!(p->port = CreateMsgPort()) ||
        !(p->req[0] = (struct AHIRequest *)CreateIORequest(p->port, sizeof(struct AHIRequest))) ||
        !(p->buf[0] = malloc(p->size * 2)) || !(p->buf[1] = malloc(p->size * 2))) {
        ahi_close(p);
        lsx_fail_errno(ft, SOX_ENOMEM, "no memory for AHI");
        return SOX_EOF;
    }
    p->req[0]->ahir_Version = 4;
    if (OpenDevice((CONST_STRPTR)AHINAME, unit, (struct IORequest *)p->req[0], 0)) {
        p->req[0]->ahir_Std.io_Device = NULL;
        ahi_close(p);
        lsx_fail_errno(ft, SOX_EOF, "cannot open ahi.device unit %lu (is AHI installed?)", (unsigned long)unit);
        return SOX_EOF;
    }
    p->open = 1;
    if (!(p->req[1] = AllocVec(sizeof(struct AHIRequest), MEMF_PUBLIC))) {
        ahi_close(p);
        lsx_fail_errno(ft, SOX_ENOMEM, "no memory for AHI");
        return SOX_EOF;
    }
    CopyMem(p->req[0], p->req[1], sizeof(struct AHIRequest));
    return SOX_SUCCESS;
}

/* plays buf[cur] (n samples), after the one playing now */
static int send(sox_format_t *ft, size_t n)
{
    priv_t *p = (priv_t *)ft->priv;
    struct AHIRequest *r = p->req[p->cur];

    r->ahir_Std.io_Message.mn_Node.ln_Pri = 0;
    r->ahir_Std.io_Command = CMD_WRITE;
    r->ahir_Std.io_Data = p->buf[p->cur];
    r->ahir_Std.io_Length = n * 2;
    r->ahir_Std.io_Offset = 0;
    r->ahir_Type = p->type;
    r->ahir_Frequency = (ULONG)(ft->signal.rate + .5);
    r->ahir_Volume = 0x10000;
    r->ahir_Position = 0x8000;
    r->ahir_Link = p->link;
    SendIO((struct IORequest *)r);
    p->busy[p->cur] = 1;
    if (p->link) {
        /* the other buffer comes free when its request is done */
        while (!CheckIO((struct IORequest *)p->link)) {
            if (Wait((1UL << p->port->mp_SigBit) | SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) {
                p->link = r;
                lsx_fail_errno(ft, SOX_EOF, "stopped (Ctrl-C)");
                return SOX_EOF;
            }
        }
        WaitIO((struct IORequest *)p->link);
        p->busy[p->cur ^ 1] = 0;
    }
    p->link = r;
    p->cur ^= 1;
    p->fill = 0;
    return SOX_SUCCESS;
}

static size_t write_samples(sox_format_t *ft, sox_sample_t const *buf, size_t len)
{
    priv_t *p = (priv_t *)ft->priv;
    size_t done = 0;
    SOX_SAMPLE_LOCALS;

    while (done < len) {
        int16_t *d = p->buf[p->cur] + p->fill;
        size_t n = len - done;
        if (n > p->size - p->fill)
            n = p->size - p->fill;
        for (size_t i = 0; i < n; i++)
            d[i] = SOX_SAMPLE_TO_SIGNED_16BIT(buf[done + i], ft->clips);
        p->fill += n;
        done += n;
        if (p->fill == p->size && send(ft, p->fill) != SOX_SUCCESS)
            return 0;
    }
    return len;
}

static int stopwrite(sox_format_t *ft)
{
    priv_t *p = (priv_t *)ft->priv;
    int rc = SOX_SUCCESS;

    if (p->fill)
        rc = send(ft, p->fill);
    if (rc == SOX_SUCCESS && p->link) {
        /* let the last buffer play out (Ctrl-C cuts it short) */
        while (!CheckIO((struct IORequest *)p->link)) {
            if (Wait((1UL << p->port->mp_SigBit) | SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)
                break;
        }
    }
    ahi_close(p);
    return rc;
}

LSX_FORMAT_HANDLER(ahi)
{
    static char const * const names[] = {"ahi", NULL};
    static unsigned const write_encodings[] = {SOX_ENCODING_SIGN2, 16, 0, 0};
    static sox_format_handler_t const handler = {SOX_LIB_VERSION_CODE,
        "Amiga AHI audio output (ahi.device)",
        names, SOX_FILE_DEVICE | SOX_FILE_NOSTDIO,
        NULL, NULL, NULL,
        startwrite, write_samples, stopwrite,
        NULL, write_encodings, NULL, sizeof(priv_t)
    };
    return &handler;
}
