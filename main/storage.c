#include "storage.h"

void nvs_init()
{
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
}

esp_err_t read_nvs_string(const char *key, char **data, size_t *length)
{
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &storage_handle);
  if (err != ESP_OK)
  {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  else
  {
    // Read
    nvs_get_str(storage_handle, key, NULL, length);
    *data = (char *)malloc(sizeof(char) * (*length));
    err = nvs_get_str(storage_handle, key, *data, length);
    switch (err)
    {
    case ESP_OK:
      printf("Done\n");
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      printf("The value is not initialized yet!\n");
      return err;
      break;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(err));
      return err;
    }
  }
  // Close
  nvs_close(storage_handle);
  return err;
}

esp_err_t write_nvs_string(const char *key, char *data)
{

  esp_err_t err = nvs_open("storage", NVS_READWRITE, &storage_handle);
  if (err != ESP_OK)
  {
    printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    return err;
  }
  else
  {
    // write
    printf("Saving: %s\n", data);
    err = nvs_set_str(storage_handle, key, data);
    switch (err)
    {
    case ESP_OK:
      printf("Done\n");
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      printf("The value is not initialized yet!\n");
      return err;
      break;
    default:
      printf("Error (%s) reading!\n", esp_err_to_name(err));
      return err;
    }
  }
  printf("Committing updates in NVS ... ");
  err = nvs_commit(storage_handle);
  printf((err != ESP_OK) ? "Failed!\n" : "Commited\n");
  // Close
  nvs_close(storage_handle);
  return err;
}