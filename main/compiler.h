#pragma once
#ifndef NP2KAI_P4_COMPILER_H
#define NP2KAI_P4_COMPILER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <limits.h>

#define SUPPORT_IA32
#define SUPPORT_PC9821
#define SUPPORT_PEGC
#define SUPPORT_IA32
#define SUPPORT_PC9821
#define SUPPORT_PEGC_HAXM
#define SUPPORT_NP2_TICKCOUNT
// Disable x86-specific features
#define USE_TSC
// #define SUPPORT_CPUID
// #define USE_MMX
// #define SUPPORT_MMX
// #define USE_FPU
// #define SUPPORT_FPU
#define BYTESEX_LITTLE
#define OSLANG_UTF8

typedef int8_t      INT8;
typedef uint8_t     UINT8;
typedef int16_t     INT16;
typedef uint16_t    UINT16;
typedef int32_t     INT32;
typedef uint32_t    UINT32;
typedef int64_t     INT64;
typedef uint64_t    UINT64;
typedef int         INT;
typedef unsigned int UINT;
typedef int8_t      SINT8;
typedef int16_t     SINT16;
typedef int32_t     SINT32;
typedef int64_t     SINT64;
typedef uint8_t     BYTE;
typedef uint16_t    WORD;
typedef uint32_t    DWORD;
typedef uint8_t     BOOL;
typedef uint8_t     BRESULT;
typedef intptr_t    INTPTR;
#define INT_PTR INTPTR

// Windows compatibility stubs
typedef union { struct { uint32_t LowPart; int32_t HighPart; }; int64_t QuadPart; } LARGE_INTEGER;
typedef uint32_t DWORD_PTR;
typedef uint8_t     REG8;
typedef uint16_t    REG16;
typedef uint8_t     PF_UINT8;
typedef uint16_t    PF_UINT16;
typedef uint32_t    PF_UINT32;
typedef char        OEMCHAR;
typedef char        CHAR;
typedef FILE*       FILEH;
typedef long        FILEPOS;
typedef long        FILELEN;
typedef void*       FLISTH;
typedef jmp_buf     sigjmp_buf;

#define FILEH_INVALID   NULL
#define FLISTH_INVALID  NULL
#define FSEEK_SET       SEEK_SET
#define FSEEK_CUR       SEEK_CUR
#define FSEEK_END       SEEK_END

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#define OEMTEXT(s)          s
#define INLINE              inline
#define STATIC_INLINE       static inline
#define CPUCALL
#define MEMCALL
#define IOOUTCALL
#define IOINPCALL
#define DMACCALL
#define TRAPCALL
#define ENVCALL
#define PARTSCALL
#define DOSIOCALL
#define FASTCALL
#define NHD_MAXSIZE2        32000

#define ZeroMemory(d,l)     memset((d),0,(l))
#define CopyMemory(d,s,l)   memcpy((d),(s),(l))
#define FillMemory(d,l,f)   memset((d),(f),(l))
#define LOADINTELDWORD(a)   (*(uint32_t*)(a))
#define LOADINTELWORD(a)    (*(uint16_t*)(a))
#define STOREINTELDWORD(a,b) (*(uint32_t*)(a)=(b))
#define STOREINTELWORD(a,b)  (*(uint16_t*)(a)=(b))

#define sigsetjmp(e,s)      setjmp(e)
#define siglongjmp(e,v)     longjmp(e,v)

#define TRACEOUT(a)         do { (void)(a); } while(0)
#define VERBOSE(a)          do { (void)(a); } while(0)
#define __ASSERT(x)

typedef struct { int x; int y; } POINT_T;
typedef struct { int left; int top; int right; int bottom; } RECT_T;
typedef struct { int type; RECT_T r; } UNIRECT;
typedef struct { UINT32 size; UINT32 sig; UINT32 ver; UINT32 param; } _COMFLAG, *COMFLAG;
typedef struct { UINT16 year; UINT8 month; UINT8 day; UINT8 week; } DOSDATE;
typedef struct { UINT8 hour; UINT8 minute; UINT8 second; UINT8 milli; } DOSTIME;
typedef struct { UINT maxitems; size_t listsize; UINT items; UINT8 *list; struct _listarray_t *laNext; } _LISTARRAY;
typedef _LISTARRAY *LISTARRAY;

#define _MALLOC(s,n)        malloc(s)
#define _MFREE(p)           free(p)

typedef struct { UINT8 dummy; } SB16;
#define SOUNDID_PC_9801_118_SB16        0xFE
#define SOUNDID_PC_9801_86_118_SB16     0xFD
#define SOUNDID_PC_9801_86_WSS_SB16     0xFC
#define SOUNDID_WSS_SB16                0xFB
#define SOUNDID_PC_9801_86_SB16         0xFA


#include "common.h"

#endif
