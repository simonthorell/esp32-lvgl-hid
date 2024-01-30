#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_common.h"
#include "esp_hardware.h"
#include "connect_wifi.h"
#include "connect_mqtt.h"
#include "gui_buttons.h"
#include "gui_textfields.h"

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    ESP_LOGI("CH", "%d ret", ret);
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

    // connect_wifi(); // Connect to wifi

    static lv_disp_drv_t disp_drv; // LVGL display driver
    static lv_disp_t *disp_handle; // LVGL display handle

    lcd_init(disp_drv, &disp_handle, false);  // initialize the LCD

    setup_buttons(); // Configure the buttons

    configure_hardware_timer(); // Configure the hardware timer

    // FreeRTOS task using second core (core 0 will be dedicated to wifi, bt etc)
    xTaskCreatePinnedToCore(ui_update_task, "update_ui", 4096 * 2, NULL, 0, NULL, 1);

    lcd_fade_in(); // Fade in the LCD for smooth startup

    setup_inputfields(); // Configure the input field styles
}