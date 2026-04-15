#pragma once
#include "compiler.h"
enum {
    SOUND_PCMSEEK   = 0,
    SOUND_PCMSEEK1  = 1,
    SOUND_PCMBEEP   = 2,
    SOUND_PCMMAX    = 3
};

BRESULT soundmng_create(UINT rate, UINT samples);
void soundmng_pcmplay(UINT num, BOOL loop);
void soundmng_pcmstop(UINT num);
void soundmng_reset(void);
void soundmng_sync(void);
void soundmng_setreverse(BOOL rev);
void soundmng_setvolume(UINT vol);
void soundmng_setrate(UINT rate);
void soundmng_destroy(void);
void soundmng_play(void);
void soundmng_stop(void);
UINT8 *soundmng_getbuf(UINT num);
void soundmng_enqueue(UINT num);
