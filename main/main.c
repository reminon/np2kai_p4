#include "compiler.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_heap_caps.h"
#include "pccore.h"
#include "np2_display.h"

extern uint8_t *mem;

static const char *TAG = "np2kai_p4";

#define SD_MOUNT_POINT  "/sdcard"
#define SD_CLK_GPIO     43
#define SD_CMD_GPIO     44
#define SD_D0_GPIO      39

OEMCHAR np2path[MAX_PATH]       = "/sdcard/np2/";
OEMCHAR np2ini[MAX_PATH]        = "/sdcard/np2/np2.cfg";
OEMCHAR np2resumeini[MAX_PATH]  = "/sdcard/np2/np2res.cfg";

static sdmmc_card_t *s_card = NULL;
static SemaphoreHandle_t s_sd_done = NULL;

static esp_err_t mount_sd(void) {
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;
    host.command_timeout_ms = 1000;  // increase from default 100ms

    sdmmc_slot_config_t slot = SDMMC_SLOT_CONFIG_DEFAULT();
    slot.width = 1;
    slot.clk   = SD_CLK_GPIO;
    slot.cmd   = SD_CMD_GPIO;
    slot.d0    = SD_D0_GPIO;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 8,
        .allocation_unit_size = 16 * 1024,
    };

    esp_err_t ret = esp_vfs_fat_sdmmc_mount(SD_MOUNT_POINT, &host, &slot, &mount_cfg, &s_card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SD mount failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "SD mounted: %s %.1fMB", s_card->cid.name,
             (float)((uint64_t)s_card->csd.capacity * s_card->csd.sector_size) / (1024*1024));
    return ESP_OK;
}

// Load a ROM file from SD into PSRAM using internal RAM bounce buffer
static size_t load_rom(const char *path, uint32_t offset, size_t maxsize) {
    static DRAM_ATTR uint8_t bounce[512];
    size_t total = 0;
    FILE *f = NULL;
    // Retry opening file - first open may fail due to SD settling
    for (int i = 0; i < 200 && !f; i++) {
        f = fopen(path, "rb");
        if (!f) esp_rom_delay_us(5000);
    }
    if (!f) return 0;
    // Read entire file with retries per chunk
    while (total < maxsize) {
        fseek(f, total, SEEK_SET);
        size_t chunk = maxsize - total;
        if (chunk > sizeof(bounce)) chunk = sizeof(bounce);
        size_t got = 0;
        for (int retry = 0; retry < 200 && got == 0; retry++) {
            got = fread(bounce, 1, chunk, f);
            if (got == 0) { esp_rom_delay_us(5000); fseek(f, total, SEEK_SET); }
        }
        if (got == 0) break;
        memcpy(mem + offset + total, bounce, got);
        total += got;
    }
    fclose(f);
    return total;
}

static void np2_task(void *arg) {
    xSemaphoreTake(s_sd_done, portMAX_DELAY);
    ESP_LOGI(TAG, "Initializing NP2kai core");
    pccore_init();
    ESP_LOGI(TAG, "mem[0xfe000]=%02x (BIOS check)", mem ? mem[0xfe000] : 0xFF);
    ESP_LOGI(TAG, "Resetting NP2kai");
    pccore_reset();
    ESP_LOGI(TAG, "NP2kai running");
    extern UINT8 (*np2_vram)[307200];
    uint16_t *fb = np2_display_get_fb();
    while (1) {
        pccore_exec(10000);
        // Blit np2_vram to display framebuffer
        if (fb && np2_vram && np2_vram[0]) {
            np2_display_update((const uint16_t*)np2_vram[0], 640, 400);
        }
        vTaskDelay(1);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "NP2kai P4 starting");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // 0. Init display
    np2_display_init();

    // 1. Allocate PSRAM buffers first (MSPI bus heavy - do before SD)
    pccore_mem_malloc();

    // 2. Mount SD after PSRAM is settled
    if (mount_sd() != ESP_OK) {
        ESP_LOGE(TAG, "SD required - halting");
        while(1) vTaskDelay(1000);
    }

    // Give MSPI bus time to settle after PSRAM alloc
    vTaskDelay(pdMS_TO_TICKS(100));

    // Wait for SD to fully settle after mount
    vTaskDelay(pdMS_TO_TICKS(500));

    // 3. Load ROMs from SD (all file I/O before np2_task starts)
    size_t n;
    const char *bios_names[] = {"bios.rom", "BIOS.ROM", NULL};
    for (int i = 0; bios_names[i]; i++) {
        char path[64]; snprintf(path, sizeof(path), "/sdcard/np2/%s", bios_names[i]);
        n = load_rom(path, 0xfe000, 0x18000);
        if (n) { ESP_LOGI(TAG, "BIOS: %s (%u bytes)", path, (unsigned)n); break; }
    }
    ESP_LOGI(TAG, "BIOS loaded: %u bytes (need 98304)", (unsigned)n);
    if (!n) ESP_LOGW(TAG, "BIOS ROM not loaded!");

    const char *itf_names[] = {"itf.rom", "ITF.ROM", NULL};
    for (int i = 0; itf_names[i]; i++) {
        char path[64]; snprintf(path, sizeof(path), "/sdcard/np2/%s", itf_names[i]);
        n = load_rom(path, 0xcc000, 0x08000);
        if (n) { ESP_LOGI(TAG, "ITF: %s (%u bytes)", path, (unsigned)n); break; }
    }

    s_sd_done = xSemaphoreCreateBinary();
    // 4. Start np2_task
    xTaskCreatePinnedToCore(np2_task, "np2kai", 32768, NULL, 5, NULL, 0);
    // Give semaphore only after all SD I/O is done
    vTaskDelay(pdMS_TO_TICKS(10));
    xSemaphoreGive(s_sd_done);
}
