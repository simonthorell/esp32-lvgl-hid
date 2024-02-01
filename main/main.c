#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "app_common.h"
#include "esp_hardware.h"
#include "esp_firmware.h"
#include "esp_usb_hid.h"
#include "connect_wifi.h"
#include "connect_mqtt.h"
#include "gui_buttons.h"
#include "gui_textfields.h"

typedef enum{
    RELEASE, // Use as false
    DEVELOP  // Use as true
} app_mode_t;

/* TODO: Fix the CMake copy-script to ensure the last built firmware .bin-file is copied.
         to the firmware folder. Also update firmware.json version as set in main.c. */
#define APPLICATION_MODE DEVELOP
#define FIRMWARE_VERSION 0.02 // Firmware version, used for FOTA (Max 2 decimal places)
#define UPDATE_JSON_URL "https://raw.githubusercontent.com/simonthorell/esp32-lvgl-hid/main/bin/firmware.json"

// Declare functions for FreeRTOS tasks
void usb_hid_task(void *pvParameters);
void wifi_task(void *pvParameters);
void mqtt_task(void *pvParameters);
void fota_task(void *pvParameters);

// Queue for HID events
QueueHandle_t app_event_queue = NULL;

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

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    ESP_LOGI("CH", "%d ret", ret);
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

    // Create a task for Wi-Fi - Priority 5 (High)
    xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, 0); 

    // Create a task for MQTT - Priority 5 (High)
    xTaskCreate(&mqtt_task, "mqtt_task", 4096, NULL, 5, 0);

    // Create a queue for HID events
    app_event_queue = xQueueCreate(10, sizeof(app_event_queue_t)); // Adjust size as necessary
    if (app_event_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create event queue");
        vTaskDelete(NULL); // Delete this task if the queue creation failed
    }

    // Create and start the USB HID task
    xTaskCreate(&usb_hid_task, "usb_hid_task", 4096, NULL, 5, NULL);

    // Create a task for FOTA - Priority 3 (Medium)
    xTaskCreate(&fota_task, "fota_task", configMINIMAL_STACK_SIZE * 4, NULL, 3, 0);

    // Configure the hardware timer
    configure_hardware_timer();
}

//==============================================================================
// FreeRTOS Tasks
//==============================================================================

void usb_hid_task(void *pvParameters) {
    // Initialize the USB Host Library
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // HID host driver configuration
    const hid_host_driver_config_t hid_host_driver_config = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .core_id = 0,
        .callback = hid_host_device_callback,
        .callback_arg = NULL
    };

    // Install HID host driver
    ESP_ERROR_CHECK(hid_host_install(&hid_host_driver_config));

    // Variables for storing events
    app_event_queue_t evt_queue;

    key_event_t key_event;

    // Main loop to handle USB events
    while (1) {
        // Simulate key press for testing
        if (APPLICATION_MODE == DEVELOP)
        while (1) {
            for (char simulated_key = 'a'; simulated_key <= 'z'; ++simulated_key) {
                // Simulate key press
                char key_char = simulated_key;

                // Handle the simulated key press as if it was a real key press
                handle_key_press_in_ui(key_char);

                // Wait between simulated key presses
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }

        // Wait for an event to be posted to the queue
        if (xQueueReceive(app_event_queue, &evt_queue, portMAX_DELAY)) {
            if (evt_queue.event_group == APP_EVENT) {
                // Handle application-specific events, such as a button press to exit

                if (evt_queue.event_group == APP_EVENT_HID_HOST) {
                    // Cast the arg to key_event_t and process
                    key_event = *(key_event_t *)evt_queue.hid_host_device.arg;

                    // Process the key event
                    if (key_event.state == KEY_STATE_PRESSED) {
                        char key_char;
                        if (get_ascii_from_keycode(key_event.modifier, key_event.key_code, &key_char)) {
                            // Send key_char to the UI
                            // Assuming you have a function to handle this
                            handle_key_press_in_ui(key_char);
                        }
                    }
                }

                // ...
            } else if (evt_queue.event_group == APP_EVENT_HID_HOST) {
                // Handle HID host events, such as device connection/disconnection or input report
                hid_host_device_event(evt_queue.hid_host_device.handle,
                                      evt_queue.hid_host_device.event,
                                      evt_queue.hid_host_device.arg);
            }
        }
    }

    // Cleanup: Uninstall HID host and USB host library
    hid_host_uninstall();
    usb_host_uninstall();
    vQueueDelete(app_event_queue);

    // Delete the task itself
    vTaskDelete(NULL);
}

void wifi_task(void *pvParameters) {
    ESP_LOGI(TAG, "Connecting to Wi-Fi...");
    connect_wifi(); // Initialize and connect to Wi-Fi

    vTaskDelete(NULL);
}

// Initialize MQTT and start the task
void mqtt_task(void *pvParameters) {
    while (wifi_connect_status == 0) {    // TODO: Replace with RTOS event group
        vTaskDelay(pdMS_TO_TICKS(1000)); // Check every second for Wi-Fi connection
    }

    ESP_LOGI(TAG, "Wi-Fi Connected. Starting MQTT...");
    mqtt_app_start();

    vTaskDelete(NULL);
}

// Firmware Update Over The Air
void fota_task(void *pvParameters) {
    while (wifi_connect_status == 0) {    // TODO: Replace with RTOS event group
        vTaskDelay(pdMS_TO_TICKS(1000)); // Check every second for Wi-Fi connection
    }

    ESP_LOGI(TAG, "Wi-Fi Connected. Starting FOTA Update Check...");
    firmware_update_check(FIRMWARE_VERSION, UPDATE_JSON_URL);

    vTaskDelete(NULL); // Clean up the task when done
}