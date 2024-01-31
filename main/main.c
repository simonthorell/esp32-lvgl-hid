#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "app_common.h"
#include "esp_hardware.h"
#include "esp_firmware.h"
// #include "esp_usb_hid.h"
#include "connect_wifi.h"
#include "connect_mqtt.h"
#include "gui_buttons.h"
#include "gui_textfields.h"

/* TODO: Fix the CMake copy-script to ensure the last built firmware .bin-file is copied.
         to the firmware folder. Also update firmware.json version as set in main.c. */
#define FIRMWARE_VERSION 0.01 // Firmware version, used for FOTA (Max 2 decimal places)
#define UPDATE_JSON_URL "https://raw.githubusercontent.com/simonthorell/esp32-lvgl-hid/main/bin/firmware.json"

// Declare functions for FreeRTOS tasks
void wifi_task(void *pvParameters);
void mqtt_task(void *pvParameters);
void fota_task(void *pvParameters);

//==============================================================================
// App Main
//==============================================================================

void app_main(void) {
    // LVGL display driver & handle
    static lv_disp_drv_t disp_drv; 
    static lv_disp_t *disp_handle;

    // Initialize the LCD
    lcd_init(disp_drv, &disp_handle, false); 

    // FreeRTOS task using second core (core 0 will be dedicated to wifi, mqtt, bt etc)
    xTaskCreatePinnedToCore(ui_update_task, "update_ui", 4096 * 2, NULL, 0, NULL, 1);

    // Setup the GUI components
    setup_buttons();
    setup_inputfields();

    // Display the firmware version on the LCD
    firmware_version_ui(FIRMWARE_VERSION);

    // Fade in the LCD for smooth startup
    lcd_fade_in();

    // Initialize USB HID keyboard
    // usb_host_hid_keyboard_init()

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    ESP_LOGI("CH", "%d ret", ret);
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

    // Create a task for Wi-Fi - Priority 5 (High)
    xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, NULL); 

    // Create a task for MQTT - Priority 5 (High)
    xTaskCreate(&mqtt_task, "mqtt_task", 4096, NULL, 5, NULL);

    // Create a task for FOTA - Priority 3 (Medium)
    xTaskCreate(&fota_task, "fota_task", configMINIMAL_STACK_SIZE * 4, NULL, 3, NULL);

    // Configure the hardware timer
    configure_hardware_timer();
}

//==============================================================================
// FreeRTOS Connectivity Tasks
//==============================================================================

void wifi_task(void *pvParameters) {
    connect_wifi(); // Initialize and connect to Wi-Fi

    vTaskDelete(NULL); // Delete the task if it's done
}

void mqtt_task(void *pvParameters) {
    while (wifi_connect_status == 0) {    // TODO: Replace with RTOS event group
        vTaskDelay(pdMS_TO_TICKS(1000)); // Check every second for Wi-Fi connection
    }

    ESP_LOGI(TAG, "Wi-Fi Connected. Starting MQTT...");
    mqtt_app_start();

    vTaskDelete(NULL); // Clean up the task when done
}

//==============================================================================
// FreeRTOS Firmware Update Task
//==============================================================================

void fota_task(void *pvParameters) {
    while (wifi_connect_status == 0) {    // TODO: Replace with RTOS event group
        vTaskDelay(pdMS_TO_TICKS(1000)); // Check every second for Wi-Fi connection
    }

    ESP_LOGI(TAG, "Wi-Fi Connected. Starting FOTA Update Check...");
    firmware_update_check(FIRMWARE_VERSION, UPDATE_JSON_URL);

    vTaskDelete(NULL); // Clean up the task when done
}