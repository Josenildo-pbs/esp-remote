#include "main.h"
#include "ir.h"
#include "storage.h"
#include <stdio.h>

static const char *TAG = "MAIN";
#define CMP(b) (strcmp(data.topic, b) == 0)
void mqtt_rx(void *params)
{
  mqtt_data_t data;
  uint8_t send;
  for (;;)
  {
    xQueueReceive(mqtt_queue, &data, portMAX_DELAY);
    ESP_LOGI(TAG, "topic: %s - msg: %s", data.topic, data.msg);
    if (CMP("/send"))
    {
      send = atoi(data.msg);
      xQueueSend(tx_queue, &send, (TickType_t)0);
    }
    if (CMP("/clone"))
    {
      send = atoi(data.msg);
      xQueueSend(rx_queue, &send, (TickType_t)0);
    }
  }
}

void events_manager(void *params)
{
  EventBits_t bits;
  for (;;)
  {

    bits = xEventGroupWaitBits(
        global_events,                           /* The event group being tested. */
        WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT, /* The bits within the event group to wait for. */
        pdTRUE,                                  /* BIT_0 & BIT_4 should be cleared before returning. */
        pdFALSE,                                 /* Don't wait for both bits, either bit will do. */
        portMAX_DELAY);                          /* Wait a maximum of 100ms for either bit to be set. */

    if ((bits & WIFI_CONNECTED_BIT) == WIFI_CONNECTED_BIT)
    {
      mqtt_init();
    }
    else if ((bits & MQTT_CONNECTED_BIT) == MQTT_CONNECTED_BIT)
    {
      xTaskCreate(mqtt_rx, "MQTT RECEIVE TASK", 2048, NULL, configMAX_PRIORITIES - 5, NULL);
    }
    else
    {
      ESP_LOGI(TAG, "EVENT NOT HANDLED!");
    }
  }
}
void heap_memory_task(void *pvParameter)
{
  for (;;)
  {
    ESP_LOGI(TAG, "Heap Memory --> %d", esp_get_free_heap_size());
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void app_main(void)
{
  // Initialize NVS
  nvs_init();

  // Global events
  global_events = xEventGroupCreate();

  // Events Task Manager
  xTaskCreate(events_manager, "events manager", 2048, NULL, configMAX_PRIORITIES - 5, NULL);
  // write_nvs_string("test", "vou me embora pra Pasargada");
  size_t str_len;
  char *str = NULL;
  if (read_nvs_string("6", &str, &str_len) == ESP_OK)
  {
    printf("\n\nstr:%s - len:%d\n\n", str, str_len);
  }
  else
  {
    printf("deu merda");
  }
  uint32_t length = 0;
  rmt_item32_t command[] = {
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      },
      {
          .duration0 = 600,
          .level0 = 0,
          .duration1 = 15000,
          .level1 = 1,

      }};
  char *json = command_to_json(command, 11);
  write_nvs_string("10", json);
  // printf("%s", json);
  // rmt_item32_t *commands = NULL;
  // json_to_command(json, &commands, &length);
  // esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(0));
  // esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(1));
  xTaskCreate(heap_memory_task, "heap_memory_task", 2048, NULL, 6, NULL);
  // printf("\n---%d-%d--\n", commands[1].duration0, commands[1].duration1);
  // Receiver  configuration
  ir_rx_config();
  // transceiver  configuration
  ir_tx_config();
  // Wifi initialization
  wifi_init();
}
