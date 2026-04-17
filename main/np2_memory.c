#include "compiler.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "pccore.h"
#include "vram/vram.h"
#include "vram/scrndraw.h"

static const char *TAG = "np2_mem";

// Declared as pointers in NP2kai source
extern UINT8 *mem;
extern UINT8 *vramex_base;
extern UINT8 *vramex;
extern UINT8 *tramupdate;
extern UINT8 *vramupdate;

// scrndraw.c pointers
extern UINT8 *np2_tram;
extern UINT8 (*np2_vram)[/* SURFACE_SIZE */];

// cpucore.c pointer
extern UINT8 *szpflag_w;

// font.c pointers
extern char *hf_buffer;

// beepc.c pointers
extern uint16_t *beep_data;
extern uint32_t *beep_time;
extern uint16_t *hf_u16buffer;
extern char *hf_bufloc;

static void *psram_alloc(size_t size, const char *name) {
    void *p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
    if (p) {
        ESP_LOGI(TAG, "Allocated %s: %u KB at %p", name, (unsigned)(size/1024), p);
    } else {
        ESP_LOGE(TAG, "FAILED to allocate %s (%u KB)", name, (unsigned)(size/1024));
    }
    return p;
}

void pccore_mem_malloc(void) {
    // Main PC-98 RAM (2MB)
    if (!mem)
        mem = psram_alloc(0x200000, "mem");

    // PEGC extended VRAM (512KB)
    if (!vramex_base) {
        vramex_base = psram_alloc(0x80000, "vramex_base");
        vramex = vramex_base;
    }

    // VRAM update tables
    if (!tramupdate)
        tramupdate = psram_alloc(0x1000, "tramupdate");
    if (!vramupdate)
        vramupdate = psram_alloc(0x8000, "vramupdate");

    // Screen buffers
    if (!np2_tram)
        np2_tram = psram_alloc(0x4b000, "np2_tram");
    if (!np2_vram)
        np2_vram = psram_alloc(0x96000 * 2, "np2_vram");

    // CPU flag table
    if (!szpflag_w)
        szpflag_w = psram_alloc(0x10000, "szpflag_w");

    // Beep buffers
    if (!beep_data)
        beep_data = psram_alloc(0x4000 * sizeof(uint16_t), "beep_data");
    if (!beep_time)
        beep_time = psram_alloc(0x4000 * sizeof(uint32_t), "beep_time");

}

void pccore_mem_free(void) {
    if (mem)         { heap_caps_free(mem);         mem = NULL; }
    if (vramex_base) { heap_caps_free(vramex_base); vramex_base = NULL; vramex = NULL; }
    if (tramupdate)  { heap_caps_free(tramupdate);  tramupdate = NULL; }
    if (vramupdate)  { heap_caps_free(vramupdate);  vramupdate = NULL; }
    if (np2_tram)    { heap_caps_free(np2_tram);    np2_tram = NULL; }
    if (np2_vram)    { heap_caps_free(np2_vram);    np2_vram = NULL; }
    if (szpflag_w)   { heap_caps_free(szpflag_w);   szpflag_w = NULL; }
}
