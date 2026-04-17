#include "compiler.h"
#include "esp_log.h"
#include "pccore.h"
#include "timemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "commng.h"
#include "fontmng.h"
#include "dosio.h"
#include "joymng.h"
#include "mousemng.h"
#include "taskmng.h"

static const char *TAG = "np2_stubs";

// ---- sysmng ----
UINT sysmng_tick(void) { return 0; }
void sysmng_update(UINT update) { (void)update; }

// ---- timemng ----
BRESULT timemng_gettime(_SYSTIME *t) {
    if (!t) return FAILURE;
    t->year=2026; t->month=4; t->day=15;
    t->hour=12; t->minute=0; t->second=0; t->milli=0; t->week=2;
    return SUCCESS;
}

// ---- scrnmng ----
BRESULT scrnmng_create(UINT8 m) { (void)m; return SUCCESS; }
void scrnmng_destroy(void) {}
void scrnmng_update(void) {}
void scrnmng_setwidth(int x, int w) { (void)x; (void)w; }
void scrnmng_setheight(int y, int h) { (void)y; (void)h; }
void scrnmng_setbpp(UINT8 b) { (void)b; }
void scrnmng_setpalettes(void) {}
RGB32 scrnmng_makepal16(RGB32 p) { return p; }
const SCRNSURF *scrnmng_surflock(void) { return NULL; }
void scrnmng_surfunlock(const SCRNSURF *s) { (void)s; }
void scrnmng_getmenu(SCRNMENU *m) { if(m){m->width=1024;m->height=600;m->bpp=16;} }

// ---- soundmng ----
BRESULT soundmng_create(UINT r, UINT s) { (void)r; (void)s; return SUCCESS; }
void soundmng_destroy(void) {}
void soundmng_play(void) {}
void soundmng_stop(void) {}
UINT8 *soundmng_getbuf(UINT n) { (void)n; return NULL; }
void soundmng_enqueue(UINT n) { (void)n; }
void soundmng_pcmplay(UINT n, BOOL l) { (void)n; (void)l; }
void soundmng_pcmstop(UINT n) { (void)n; }

// ---- joymng ----
REG8 joymng_getstat(void) { return 0xFF; }
void joymng_sync(void) {}

// ---- mousemng ----
void mousemng_sync(void) {}

// ---- taskmng ----
void taskmng_exit(void) {}
BOOL taskmng_isavail(void) { return FALSE; }

// ---- commng ----
COMMNG commng_create(UINT d, BOOL r) { (void)d; (void)r; return NULL; }
void commng_destroy(COMMNG h) { (void)h; }

// ---- fontmng ----
FNTDAT fontmng_create(int s, UINT t, const OEMCHAR *f) { (void)s;(void)t;(void)f; return NULL; }
void fontmng_destroy(FNTDAT f) { (void)f; }
BRESULT fontmng_getsize(FNTDAT f, const OEMCHAR *s, POINT_T *p) { (void)f;(void)s; if(p){p->x=8;p->y=16;} return SUCCESS; }
BRESULT fontmng_getdrawsize(FNTDAT f, const OEMCHAR *s, POINT_T *p) { return fontmng_getsize(f,s,p); }
BRESULT fontmng_getfont(FNTDAT f, const OEMCHAR *s, UINT8 *b) { (void)f;(void)s;(void)b; return FAILURE; }

// ---- dosio ----
FILEH file_open(const OEMCHAR *p) { return (FILEH)fopen(p,"r+b"); }
FILEH file_open_rb(const OEMCHAR *p) {
    if (!p || p[0] == '\0') {
        // Print backtrace hint
        printf("file_open_rb: empty path! caller=0x%08x\n", (unsigned)__builtin_return_address(0));
        return FILEH_INVALID;
    }

    FILE *f = fopen(p, "rb");
    printf("file_open_rb(%s) = %p\n", p, f);
    return (FILEH)f;
}
FILEH file_create(const OEMCHAR *p) { return (FILEH)fopen(p,"w+b"); }
void  file_close(FILEH h) { if(h) fclose((FILE*)h); }
UINT file_read(FILEH h, void *b, UINT s) {
    // Use bounce buffer in internal RAM for SDMMC DMA compatibility
    static DRAM_ATTR uint8_t bounce[4096];
    UINT total = 0;
    uint8_t *dst = (uint8_t*)b;
    printf("file_read: dst=%p size=%u\n", b, s);
    while (total < s) {
        UINT chunk = s - total;
        if (chunk > sizeof(bounce)) chunk = sizeof(bounce);
        UINT got = (UINT)fread(bounce, 1, chunk, (FILE*)h);
        if (got == 0) break;
        memcpy(dst + total, bounce, got);
        total += got;
    }
    return total;
}
UINT  file_write(FILEH h, const void *b, UINT s) { return (UINT)fwrite(b,1,s,(FILE*)h); }
long  file_seek(FILEH h, long o, int w) { fseek((FILE*)h,o,w); return ftell((FILE*)h); }
UINT32 file_getsize(FILEH h) {
    long p=ftell((FILE*)h); fseek((FILE*)h,0,SEEK_END);
    long s=ftell((FILE*)h); fseek((FILE*)h,p,SEEK_SET); return (UINT32)s;
}
void file_getdatetime(FILEH h, DOSDATE *d, DOSTIME *t) { (void)h;(void)d;(void)t; }
UINT file_attr(const OEMCHAR *p) { (void)p; return 0; }
BRESULT file_delete(const OEMCHAR *p) { (void)p; return FAILURE; }
BRESULT file_rename(const OEMCHAR *o, const OEMCHAR *n) { (void)o;(void)n; return FAILURE; }
BRESULT file_dircreate(const OEMCHAR *p) { (void)p; return FAILURE; }
BRESULT file_dirdelete(const OEMCHAR *p) { (void)p; return FAILURE; }
FLISTH file_findopen(const OEMCHAR *p) { (void)p; return FLISTH_INVALID; }
BRESULT file_findnext(FLISTH h, FLINFO *i) { (void)h;(void)i; return FAILURE; }
void file_findclose(FLISTH h) { (void)h; }

// ---- np2 globals ----
void np2_dosreset(void) {}
void np2_reset(void) { pccore_reset(); }
void np2_close(void) {}

// ---- cpumem stubs ----
#include "i386c/cpumem.h"
REG16 MEMCALL meml_read16(UINT32 address) {
    extern UINT8 *mem;
    if (mem && address + 1 < 0x200000)
        return *(REG16*)(mem + address);
    return 0;
}

// ---- dosio path stubs ----
void file_catname(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {
    if (!dst || !src) return;
    int dlen = strlen(dst);
    if (dlen > 0 && dst[dlen-1] != '/') {
        if (dlen < maxlen-1) { dst[dlen] = '/'; dst[dlen+1] = 0; dlen++; }
    }
    strncat(dst, src, maxlen - dlen - 1);
}

// ---- cpu debug stubs ----
const char *cpu_reg2str(void) { return ""; }
void cpu_disasm(char *buf, int len) { (void)buf; (void)len; }

// ---- dosio path functions ----
void file_cpyname(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {
    strncpy(dst, src, maxlen-1);
    dst[maxlen-1] = 0;
}

void file_cutname(OEMCHAR *path) {
    // Remove filename, keep directory
    char *p = strrchr(path, '/');
    if (p) *(p+1) = 0;
    else path[0] = 0;
}

void file_cutext(OEMCHAR *path) {
    char *p = strrchr(path, '.');
    if (p) *p = 0;
}

void file_setext(OEMCHAR *path, const OEMCHAR *ext, int maxlen) {
    file_cutext(path);
    strncat(path, ext, maxlen - strlen(path) - 1);
}

BRESULT file_getfattr(const OEMCHAR *path) { (void)path; return FAILURE; }

// ---- fontmng stubs ----
FNTDAT fontmng_get(int size, UINT type, const OEMCHAR *face) {
    return fontmng_create(size, type, face);
}

// ---- more dosio path functions ----
BOOL file_cmpname(const OEMCHAR *a, const OEMCHAR *b) {
    return strcasecmp(a, b) == 0 ? TRUE : FALSE;
}

OEMCHAR *file_getext(const OEMCHAR *path) {
    const char *p = strrchr(path, '.');
    return (OEMCHAR *)(p ? p : "");
}

OEMCHAR *file_getname(const OEMCHAR *path) {
    const char *p = strrchr(path, '/');
    return (OEMCHAR *)(p ? p+1 : path);
}

static OEMCHAR s_cdpath[MAX_PATH];
OEMCHAR *file_getcd(const OEMCHAR *filename) {
    snprintf(s_cdpath, sizeof(s_cdpath), "/sdcard/np2/%s", filename ? filename : "");
    return s_cdpath;
}

void file_setcd(const OEMCHAR *path) { (void)path; }

// ---- oemtext stubs ----
void oemtext_sjistooem(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {
    strncpy(dst, src, maxlen-1);
    dst[maxlen-1] = 0;
}

void oemtext_oemtosjis(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {
    strncpy(dst, src, maxlen-1);
    dst[maxlen-1] = 0;
}

// ---- sysmng additional stubs ----
void sysmng_fddaccess(UINT drv) { (void)drv; }
void sysmng_hddaccess(UINT drv) {}
void sysmng_cpureset(void) {}

// ---- msgbox stub ----
void msgbox(const OEMCHAR *title, const OEMCHAR *msg) {
    (void)title;
    printf("MSGBOX: %s\n", msg ? msg : "");
}

// ---- GETTICK stub ----
#include <esp_timer.h>
UINT32 GETTICK(void) {
    return (UINT32)(esp_timer_get_time() / 1000);
}

// ---- i386hax stubs (we use software emulation, not HAXM) ----
BOOL i386hax_check(void) { return FALSE; }
void i386hax_initialize(void) {}
void i386hax_createVM(void) {}
void i386hax_resetVMCPU(void) {}
void i386hax_destroy(void) {}
void i386hax_freemem(void) {}

// ---- file_setseparator stub ----
void file_setseparator(OEMCHAR *path, int maxlen) { (void)path; (void)maxlen; }

// ---- milstr stubs ----
#include <strings.h>
int milstr_cmp(const char *a, const char *b) { return strcasecmp(a, b); }
int milstr_ncmp(const char *a, const char *b, int n) { return strncasecmp(a, b, n); }
char *milstr_chr(const char *s, int c) { return strchr(s, c); }
char *milstr_skip(const char *s) { while(*s==' '||*s=='\t') s++; return (char*)s; }

// ---- i386hax additional stubs ----
void i386hax_vm_setmemoryarea(UINT32 a, void *p, UINT32 s, UINT f) { (void)a;(void)p;(void)s;(void)f; }
void i386hax_vm_run(void) {}
void i386hax_vm_setregisters(void) {}
void i386hax_vm_getregisters(void) {}

// ---- soundmng additional stubs ----
void soundmng_reset(void) {}
void soundmng_sync(void) {}
void soundmng_setreverse(BOOL rev) { (void)rev; }
void soundmng_setvolume(UINT vol) { (void)vol; }
void soundmng_setrate(UINT rate) { (void)rate; }

// ---- scrnmng additional stubs ----
BOOL scrnmng_haveextend(void) { return FALSE; }
void scrnmng_topwinrequest(void) {}
void scrnmng_fullscrnrequest(void) {}
UINT8 scrnmng_getbpp(void) { return 16; }

// ---- np2hax structs (dummy) ----
#include <stdarg.h>
typedef struct { int enable; int vm; int vcpu; } _NP2HAX;
typedef struct { int s; } _NP2HAXSTAT;
typedef struct { int c; } _NP2HAXCORE;
_NP2HAX np2hax = {0};
_NP2HAXSTAT np2haxstat = {0};
_NP2HAXCORE np2haxcore = {0};

// ---- more i386hax stubs ----
void i386hax_vm_allocmemory(void) {}
void i386hax_vm_setmemory(void) {}
void i386hax_vm_setbankmemory(void) {}
void i386hax_vm_setextmemory(void) {}
void i386hax_vm_removememoryarea(UINT32 a, UINT32 s) { (void)a;(void)s; }
void i386hax_resetVMMem(void) {}
void i386hax_vm_exec(void) {}
void i386haxfunc_vcpu_setREGs(void) {}
void i386haxfunc_vcpu_setFPU(void) {}
void i386haxfunc_vcpu_getREGs(void) {}
void i386haxfunc_vcpu_getFPU(void) {}
void ia32hax_copyregHAXtoNP2(void) {}

// ---- lio stubs ----
void lio_initialize(void) {}
void bios_lio(void) {}

// ---- wabrly stubs ----
void wabrly_initialize(void) {}

// ---- mouse stubs ----
typedef struct { int x; int y; UINT8 btn; } MOUSESTAT;
void mousemng_getstat(MOUSESTAT *s) { if(s){s->x=0;s->y=0;s->btn=0;} }

// ---- va stubs (should come from stdarg but just in case) ----
// va_start/va_end are macros - if missing it means stdarg.h not included
// Add include to interface.c instead

// ---- milstr additional ----
void milstr_ncpy(OEMCHAR *dst, const OEMCHAR *src, int maxlen) {
    strncpy(dst, src, maxlen-1); dst[maxlen-1]=0;
}

// ---- OEMSPRINTF / STRLEN macros as functions ----
int OEMSPRINTF(OEMCHAR *buf, const OEMCHAR *fmt, ...) {
    va_list ap; va_start(ap,fmt);
    int r = vsnprintf(buf, 256, fmt, ap);
    va_end(ap); return r;
}
int STRLEN(const OEMCHAR *s) { return (int)strlen(s); }

// ---- scrnmng additional ----
void scrnmng_setextend(UINT8 ext) { (void)ext; }

// ---- fdd stubs ----
void fdd_fdcresult(void) {}
