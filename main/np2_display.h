#pragma once
#include "compiler.h"
#include "esp_err.h"

esp_err_t np2_display_init(void);
void np2_display_update(const uint16_t *src, int w, int h);
uint16_t *np2_display_get_fb(void);
