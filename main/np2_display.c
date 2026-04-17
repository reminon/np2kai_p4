#include "compiler.h"
#include "np2_display.h"
#include "esp_log.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ek79007.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_ldo_regulator.h"
#include "esp_task_wdt.h"
#include "esp_heap_caps.h"

static const char *TAG = "np2_disp";

#define H_SIZE          1024
#define V_SIZE          600
#define BITS_PER_PIXEL  16
#define BACKLIGHT_GPIO  31

static esp_lcd_panel_handle_t    s_panel = NULL;
static esp_lcd_dsi_bus_handle_t  s_dsi_bus = NULL;
static esp_lcd_panel_io_handle_t s_dbi_io = NULL;
static uint16_t *s_fb = NULL;
static esp_ldo_channel_handle_t s_ldo3 = NULL;
static esp_ldo_channel_handle_t s_ldo4 = NULL;

esp_err_t np2_display_init(void) {
    esp_err_t err;

    // Enable LDOs for display panel
    esp_ldo_channel_config_t ldo3_cfg = { .chan_id = 3, .voltage_mv = 2500 };
    err = esp_ldo_acquire_channel(&ldo3_cfg, &s_ldo3);
    if (err != ESP_OK) { ESP_LOGE(TAG, "LDO3: %s", esp_err_to_name(err)); return err; }
    esp_ldo_channel_config_t ldo4_cfg = { .chan_id = 4, .voltage_mv = 3300 };
    err = esp_ldo_acquire_channel(&ldo4_cfg, &s_ldo4);
    if (err != ESP_OK) { ESP_LOGE(TAG, "LDO4: %s", esp_err_to_name(err)); return err; }
    ESP_LOGI(TAG, "LDO3+LDO4 enabled");
    vTaskDelay(pdMS_TO_TICKS(50)); // Let LDOs stabilize

    // Backlight
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = 1000,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer);
    ledc_channel_config_t ch = {
        .gpio_num   = BACKLIGHT_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 512,
        .hpoint     = 0,
    };
    ledc_channel_config(&ch);

    // DSI bus
    esp_lcd_dsi_bus_config_t bus_cfg = {
        .bus_id             = 0,
        .num_data_lanes     = 2,
        .phy_clk_src        = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = 900,
    };
    err = esp_lcd_new_dsi_bus(&bus_cfg, &s_dsi_bus);
    if (err != ESP_OK) { ESP_LOGE(TAG, "DSI bus: %s", esp_err_to_name(err)); return err; }

    // DBI IO
    esp_lcd_dbi_io_config_t dbi_cfg = {
        .virtual_channel = 0,
        .lcd_cmd_bits    = 8,
        .lcd_param_bits  = 8,
    };
    err = esp_lcd_new_panel_io_dbi(s_dsi_bus, &dbi_cfg, &s_dbi_io);
    if (err != ESP_OK) { ESP_LOGE(TAG, "DBI IO: %s", esp_err_to_name(err)); return err; }

    // DPI panel
    esp_lcd_dpi_panel_config_t dpi_cfg = {
        .dpi_clk_src        = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = 51,
        .virtual_channel    = 0,
        .pixel_format       = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .num_fbs            = 1,
        .video_timing = {
            .h_size            = H_SIZE,
            .v_size            = V_SIZE,
            .hsync_back_porch  = 160,
            .hsync_pulse_width = 70,
            .hsync_front_porch = 160,
            .vsync_back_porch  = 23,
            .vsync_pulse_width = 10,
            .vsync_front_porch = 12,
        },
        .flags.use_dma2d = true,
    };
    ek79007_vendor_config_t vendor_cfg = {
        .mipi_config = { .dsi_bus = s_dsi_bus, .dpi_config = &dpi_cfg },
    };
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = -1,
        .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = BITS_PER_PIXEL,
        .vendor_config  = &vendor_cfg,
    };
    err = esp_lcd_new_panel_ek79007(s_dbi_io, &panel_cfg, &s_panel);
    if (err != ESP_OK) { ESP_LOGE(TAG, "EK79007: %s", esp_err_to_name(err)); return err; }

    esp_task_wdt_reset();
    esp_lcd_panel_reset(s_panel);
    esp_task_wdt_reset();
    esp_lcd_panel_init(s_panel);
    esp_task_wdt_reset();
    esp_lcd_panel_disp_on_off(s_panel, true);

    // Get framebuffer
    esp_lcd_dpi_panel_get_frame_buffer(s_panel, 1, (void**)&s_fb);
    ESP_LOGI(TAG, "Display init OK, fb=%p", s_fb);

    // Clear to black
    if (s_fb) memset(s_fb, 0, H_SIZE * V_SIZE * 2);

    return ESP_OK;
}

void np2_display_update(const uint16_t *src, int w, int h) {
    if (!s_fb || !src) return;
    // Center PC-98 640x400 on 1024x600 display
    int x_off = (H_SIZE - w) / 2;
    int y_off = (V_SIZE - h) / 2;
    for (int y = 0; y < h; y++) {
        memcpy(s_fb + (y + y_off) * H_SIZE + x_off, src + y * w, w * 2);
    }
}

uint16_t *np2_display_get_fb(void) { return s_fb; }
