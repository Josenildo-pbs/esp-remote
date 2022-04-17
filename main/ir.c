#include "ir.h"
static rmt_channel_t example_tx_channel = RMT_CHANNEL_0;
static rmt_channel_t example_rx_channel = RMT_CHANNEL_2;
char *command_to_json(rmt_item32_t *data, size_t length)
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

void json_to_command(char *str, rmt_item32_t **data)
{
  cJSON *obj = cJSON_Parse(str);
  cJSON *commands = NULL;
  cJSON *command = NULL;
  commands = cJSON_GetObjectItemCaseSensitive(obj, "commands");
  uint32_t i = 0;
  *data = (rmt_item32_t *)malloc(sizeof(cJSON_GetArraySize(commands) * sizeof(rmt_item32_t)));
  cJSON_ArrayForEach(command, commands)
  {
    (*data)[i].val = command->valuedouble;
    i++;
  }
  cJSON_free(obj);
}

ir_tx_config()
{
}