#pragma once
#include "compiler.h"

// NP2 global config
#define NP2VER_MAJOR    0
#define NP2VER_MINOR    86
#define NP2VER_PATCH    0

extern OEMCHAR np2path[MAX_PATH];
extern OEMCHAR np2ini[MAX_PATH];
extern OEMCHAR np2resumeini[MAX_PATH];

void np2_dosreset(void);
void np2_reset(void);
void np2_close(void);
