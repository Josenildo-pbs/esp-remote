#ifndef STORAGE_H
#define STORAGE_H
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
nvs_handle_t storage_handle;
void nvs_init();
esp_err_t read_nvs_string(const char *key, char **data, size_t *length);
esp_err_t write_nvs_string(const char *key, char *data);
#endif