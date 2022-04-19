#ifndef IR_H
#define IR_H
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/rmt.h"
#include "esp_log.h"
#include "storage.h"
#include "mqtt.h"

typedef struct
{
  uint32_t duration0;
  uint32_t level0;
  uint32_t duration1;
  uint32_t level1;
  uint32_t val;
} micha_t;

QueueHandle_t rx_queue;
QueueHandle_t tx_queue;

char *command_to_json(rmt_item32_t *data, uint32_t length);
void json_to_command(char *str, rmt_item32_t **data, uint32_t *length);
void ir_tx_config();
void ir_rx_config();
#endif