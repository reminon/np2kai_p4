#pragma once
#include "compiler.h"

typedef struct {
    UINT8  *ptr;
    int     width;
    int     height;
    int     bpp;
    int     xalign;
    int     yalign;
} SCRNSURF;

typedef struct {
    int     width;
    int     height;
    int     bpp;
} SCRNMENU;

BRESULT scrnmng_create(UINT8 scrnmode);
void scrnmng_destroy(void);
void scrnmng_update(void);
void scrnmng_setwidth(int posx, int width);
void scrnmng_setheight(int posy, int height);
void scrnmng_setbpp(UINT8 bpp);
void scrnmng_setpalettes(void);
RGB32 scrnmng_makepal16(RGB32 pal32);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);
void scrnmng_getmenu(SCRNMENU *menu);
BOOL scrnmng_haveextend(void);
void scrnmng_topwinrequest(void);
void scrnmng_fullscrnrequest(void);
UINT8 scrnmng_getbpp(void);
