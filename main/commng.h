#pragma once
#include "compiler.h"

enum { COMCONNECT_OFF=0, COMCONNECT_SERIAL=1, COMCONNECT_MIDI=2, COMCONNECT_PARALLEL=3 };
enum { COMCREATE_SERIAL=0, COMCREATE_PC9861K1=1, COMCREATE_PC9861K2=2, COMCREATE_PRINTER=3, COMCREATE_MPU98II=4 };
enum { COMMSG_MIDIRESET=0, COMMSG_SETFLAG=1, COMMSG_GETFLAG=2, COMMSG_REQSEND=3, COMMSG_GETERROR=4, COMMSG_CLRERROR=5, COMMSG_CHANGESPEED=6, COMMSG_CHANGEMODE=7, COMMSG_DTRCTS=8, COMMSG_PURGE=9, COMMSG_SETCOMMAND=10 };

typedef struct _commng _COMMNG;
typedef struct _commng *COMMNG;
struct _commng {
    UINT connect;
    UINT (*read)(COMMNG self, UINT8 *data);
    UINT (*write)(COMMNG self, UINT8 data);
    UINT (*writeretry)(COMMNG self);
    UINT8 (*getstat)(COMMNG self);
    INTPTR (*msg)(COMMNG self, UINT msg, INTPTR param);
    UINT (*lastwritesuccess)(COMMNG self);
    void (*beginblocktranster)(COMMNG self);
    void (*endblocktranster)(COMMNG self);
    void (*release)(COMMNG self);
    UINT8 lastdata;
    UINT8 lastdatafail;
    UINT lastdatatime;
};

typedef struct { UINT32 size; UINT32 sig; UINT32 ver; UINT32 param; } _COMFLAG2, *COMFLAG2;

COMMNG commng_create(UINT device, BOOL onReset);
void commng_destroy(COMMNG hdl);
