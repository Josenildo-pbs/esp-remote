#ifndef IR_H
#define IR_H
#include "cJSON.h"
#include "driver/rmt.h"
typedef struct
{
  uint32_t duration0;
  uint32_t level0;
  uint32_t duration1;
  uint32_t level1;
  uint32_t val;
} micha_t;

char *command_to_json(rmt_item32_t *data, size_t length);
void json_to_command(char *str, rmt_item32_t **data);

#endif