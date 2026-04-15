#pragma once
#include "compiler.h"

typedef struct {
    int width;
    int height;
    int bpp;
    UINT8 *bitmap;
} _FNTDAT;
typedef _FNTDAT* FNTDAT;

#define FDAT_PROPORTIONAL   0x01
#define FDAT_ITALIC         0x02
#define FDAT_BOLD           0x04

FNTDAT fontmng_create(int size, UINT type, const OEMCHAR *fontface);
void fontmng_destroy(FNTDAT font);
BRESULT fontmng_getsize(FNTDAT font, const OEMCHAR *str, POINT_T *pt);
BRESULT fontmng_getdrawsize(FNTDAT font, const OEMCHAR *str, POINT_T *pt);
BRESULT fontmng_getfont(FNTDAT font, const OEMCHAR *str, UINT8 *bitmap);
