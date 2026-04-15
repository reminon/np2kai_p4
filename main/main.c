#include "compiler.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_heap_caps.h"
#include "pccore.h"

static const char *TAG = "np2kai_p4";

#define SD_MOUNT_POINT  "/sdcard"
#define SD_CLK_GPIO     43
#define SD_CMD_GPIO     44
#define SD_D0_GPIO      39

// NP2kai path globals
OEMCHAR np2path[MAX_PATH] = "/sdcard/np2/";
OEMCHAR np2ini[MAX_PATH]  = "/sdcard/np2/np2.cfg";
OEMCHAR np2resumeini[MAX_PATH] = "/sdcard/np2/np2res.cfg";

static sdmmc_card_t *s_card = NULL;

static esp_err_t mount_sd(void) {
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_DEFAULT;

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
    ESP_LOGI(TAG, "SD mounted: %s %.1fMB",
             s_card->cid.name,
             (float)((uint64_t)s_card->csd.capacity * s_card->csd.sector_size) / (1024*1024));
    return ESP_OK;
}

static void np2_task(void *arg) {
    ESP_LOGI(TAG, "Allocating PSRAM buffers");
    pccore_mem_malloc();

    ESP_LOGI(TAG, "Initializing NP2kai core");
    pccore_init();

    ESP_LOGI(TAG, "Resetting NP2kai");
    pccore_reset();

    ESP_LOGI(TAG, "NP2kai running");
    while (1) {
        pccore_exec(1000);
        vTaskDelay(1);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "NP2kai P4 starting");

    // Init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Mount SD
    if (mount_sd() != ESP_OK) {
        ESP_LOGE(TAG, "SD required - halting");
        while(1) vTaskDelay(1000);
    }

    // Run NP2kai on a dedicated task with large stack
    xTaskCreatePinnedToCore(np2_task, "np2kai", 8192, NULL, 5, NULL, 0);
}
