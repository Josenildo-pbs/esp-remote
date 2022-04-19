
#ifndef MQTT_H
#define MQTT_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "event.h"

#define BROKER_URL "mqtt://mqtt.eclipseprojects.io"
#define BROKER_USER "1234"
#define BROKER_PASS "1234"

typedef struct
{
  char topic[100];
  char msg[100];
} mqtt_data_t;

QueueHandle_t mqtt_queue;
esp_mqtt_client_handle_t client;
void mqtt_init(void);

#endif