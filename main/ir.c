#include "ir.h"

#define RMT_TX_GPIO 18
#define RMT_RX_GPIO 19
static const char *TAG = "IR";
rmt_channel_t tx_channel = RMT_CHANNEL_0;
rmt_channel_t rx_channel = RMT_CHANNEL_2;
RingbufHandle_t rb = NULL;
char *command_to_json(rmt_item32_t *data, uint32_t length)
{
  cJSON *obj = NULL;
  cJSON *len = NULL;
  cJSON *commands = NULL;
  cJSON *command = NULL;
  char *str = NULL;

  obj = cJSON_CreateObject();
  len = cJSON_CreateNumber(length);
  cJSON_AddItemToObject(obj, "length", len);
  commands = cJSON_CreateArray();
  cJSON_AddItemToObject(obj, "commands", commands);
  for (uint32_t i = 0; i < length; i++)
  {
    command = cJSON_CreateNumber(data[i].val);
    cJSON_AddItemToArray(commands, command);
  }
  str = cJSON_Print(obj);
  cJSON_free(obj);
  cJSON_free(command);
  return str;
}

void json_to_command(char *str, rmt_item32_t **data, uint32_t *length)
{
  cJSON *obj = cJSON_Parse(str);
  cJSON *commands = NULL;
  cJSON *command = NULL;
  commands = cJSON_GetObjectItemCaseSensitive(obj, "commands");
  *length = cJSON_GetArraySize(commands);
  uint32_t i = 0;
  *data = (rmt_item32_t *)malloc(sizeof(cJSON_GetArraySize(commands) * sizeof(rmt_item32_t)));
  cJSON_ArrayForEach(command, commands)
  {
    (*data)[i].val = command->valuedouble;
    i++;
  }
  cJSON_free(obj);
  cJSON_free(commands);
  cJSON_free(command);
}
void tx_task(void *params)
{
  rmt_item32_t *items = NULL;
  uint32_t length;
  uint8_t data;
  char *str = NULL;
  size_t str_len;
  char key[5];
  // items = (rmt_item32_t *)malloc(length * sizeof(rmt_item32_t));

  for (;;)
  {
    xQueueReceive(tx_queue, &data, portMAX_DELAY);
    ESP_LOGI(TAG, "TX: %d", data);
    sprintf(key, "%d", data);
    if (read_nvs_string(key, &str, &str_len) == ESP_OK)
    {
      ESP_LOGI(TAG, "\n\nstr:%s - len:%d\n\n", str, str_len);
      json_to_command(str, &items, &length);
      rmt_write_items(tx_channel, items, length, false);
    }
    else
    {
      ESP_LOGI(TAG, "String reading failed!");
    }
    free(str);
    free(items);
  }
}
void ir_tx_config()
{
  rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO, tx_channel);
  rmt_tx_config.tx_config.carrier_en = true;
  rmt_tx_config.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
  rmt_tx_config.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;
  rmt_tx_config.tx_config.idle_output_en = true;
  rmt_driver_install(tx_channel, 0, 0);
  rmt_config(&rmt_tx_config);
  // transceiver Queue
  tx_queue = xQueueCreate(5, sizeof(uint8_t));
  // transceiver task
  xTaskCreate(tx_task, "tx task", 10 * 2048, NULL, configMAX_PRIORITIES - 5, NULL);
}

void rx_task(void *params)
{
  uint8_t data;
  uint32_t length;
  rmt_item32_t *items = NULL;
  char key[5];
  for (;;)
  {
    xQueueReceive(rx_queue, &data, portMAX_DELAY);
    // Start receive
    esp_mqtt_client_stop(client);
    rmt_rx_start(rx_channel, true);
    items = (rmt_item32_t *)xRingbufferReceive(rb, &length, portMAX_DELAY); // 6000 / portTICK_PERIOD_MS
    for (int i = 0; i < 3; i++)
    {
      ESP_LOGI(TAG, "{low: %d - levelLow: %d - high: %d - levelHigh: %d }\n", items[i].duration0, items[i].level0, items[i].duration1, items[i].level1);
    }
    sprintf(key, "%d", data);
    write_nvs_string(key, command_to_json(items, length));
    rmt_rx_stop(rx_channel);
    vRingbufferReturnItem(rb, (void *)items);
    esp_mqtt_client_start(client);
  }
  rmt_driver_uninstall(rx_channel);
  vTaskDelete(NULL);
}

void ir_rx_config()
{

  rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(RMT_RX_GPIO, rx_channel);
  rmt_rx_config.mem_block_num = 2;
  rmt_config(&rmt_rx_config);
  rmt_driver_install(rx_channel, 10000, 0);
  // get RMT RX ringbuffer
  rmt_get_ringbuf_handle(rx_channel, &rb);
  assert(rb != NULL);

  // Receiver Queue
  rx_queue = xQueueCreate(5, sizeof(uint8_t));
  // Receiver Task
  xTaskCreate(rx_task, "rx task", 10 * 2048, NULL, configMAX_PRIORITIES - 5, NULL);
}
