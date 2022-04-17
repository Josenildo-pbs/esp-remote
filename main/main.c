#include "main.h"
#include "ir.h"
#include "storage.h"
#include <stdio.h>

static const char *TAG = "MAIN";

void mqtt_rx(void *params)
{
  mqtt_data_t data;
  for (;;)
  {
    xQueueReceive(mqtt_queue, &data, portMAX_DELAY);
    ESP_LOGI(TAG, "topic: %s - msg: %s", data.topic, data.msg);
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
  if (read_nvs_string("test", &str, &str_len) == ESP_OK)
  {
    printf("\n\nstr:%s - len:%d\n\n", str, str_len);
  }
  else
  {
    printf("deu merda");
  }
  rmt_item32_t command[3] = {
      {
          .duration0 = 12,
          .level0 = 0,
          .duration1 = 34,
          .level1 = 1,

      },
      {
          .duration0 = 67,
          .level0 = 0,
          .duration1 = 89,
          .level1 = 1,

      },
      {
          .duration0 = 92,
          .level0 = 0,
          .duration1 = 319,
          .level1 = 1,

      }};
  char *json = command_to_json(command, 3);
  printf("%s", json);
  rmt_item32_t *commands = NULL;
  json_to_command(json, &commands);

  printf("\n---%d-%d--\n", commands[1].duration0, commands[1].duration1);
  // Wifi initialization
  wifi_init();
}
