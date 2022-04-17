#ifndef EVENT_H
#define EVENT_H
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MQTT_CONNECTED_BIT BIT2

EventGroupHandle_t global_events;

#endif