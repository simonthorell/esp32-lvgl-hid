#ifndef ESP_FIRMWARE_H
#define ESP_FIRMWARE_H

#include <stdio.h>              // Standard C library for printf, sprintf, etc.
#include <string.h>             // Standard C library for string operations
#include "esp_system.h"         // ESP32 system header
#include "esp_log.h"            // ESP32 logging library
#include "esp_http_client.h"    // ESP32 HTTP client library
#include "esp_https_ota.h"      // ESP32 HTTPS OTA (Over-The-Air) library
#include "cJSON.h"              // cJSON library for JSON parsing
#include "esp_event.h"
#include "esp_crt_bundle.h"     // ESP32 certificate bundle (mozilla_root_certs)

esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void firmware_update_check(double FIRMWARE_VERSION, const char* UPDATE_JSON_URL);

#endif // ESP_FIRMWARE_H
