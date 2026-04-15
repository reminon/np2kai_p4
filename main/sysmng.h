#pragma once
#include "compiler.h"
#define SYS_UPDATECFG   0x0001
#define SYS_UPDATEHDD   0x0002
#define SYS_UPDATEFDD   0x0004
#define SYS_UPDATEOSCILLOSCOPE 0x0008
#define SYS_UPDATECLOCK 0x0010
#define SYS_UPDATERATE  0x0020
#define SYS_UPDATESBUF  0x0040
#define SYS_UPDATEOSCFG 0x0080
UINT sysmng_tick(void);
void sysmng_update(UINT update);
void sysmng_fddaccess(UINT drv);
void sysmng_hddaccess(UINT drv);
void sysmng_cpureset(void);
