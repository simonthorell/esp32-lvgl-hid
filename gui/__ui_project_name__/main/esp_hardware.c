#include "esp_hardware.h"

// Define battery status variables
int battery_voltage;
int battery_percentage;
bool on_usb_power = false;

// Define LCD Screen brightness variables
int last_screen_brightness_step = 16;
int screen_brightness_step = 16;
int current_battery_symbol_idx = 0;

void update_hw_info_timer_cb(void *arg) {
    battery_voltage = get_battery_voltage();
    on_usb_power = usb_power_voltage(battery_voltage);
    battery_percentage = (int) volts_to_percentage((double) battery_voltage / 1000);
}

void update_ui() {
    // Perform your ui updates here
    ESP_LOGI(TAG, "update_ui function call");
}

void ui_update_task(void *pvParam) {
    lvgl_port_lock(0);
    ui_init();
    lvgl_port_unlock();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(50)); // Update UI every 50ms
        if (lvgl_port_lock(0)) {
            update_ui(); // Update the UI under the lvgl semaphore lock
            lvgl_port_unlock();
        }
    }
    // FreeRTOS task should not return
}

// Blocking action for LCD brightness fade in
void lcd_fade_in() {
    vTaskDelay(pdMS_TO_TICKS(100));
    screen_brightness_step = 0;
    last_screen_brightness_step = 0;
    lcd_brightness_set(screen_brightness_step);
    vTaskDelay(pdMS_TO_TICKS(50));

    for (int i = 0; i <= 16; i++) {
        screen_brightness_step++;
        last_screen_brightness_step++;
        lcd_brightness_set(ceil(screen_brightness_step * (int) (100 / (float) 16)));
        vTaskDelay(pdMS_TO_TICKS(70));
    }
}

// Configure a periodic timer to update the battery voltage, brightness level etc
void configure_hardware_timer() {
    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &update_hw_info_timer_cb,
            .name = "update_hw_info_timer"
    };

    esp_timer_handle_t update_hw_info_timer_handle;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &update_hw_info_timer_handle));
    // update the hw info every 250 milliseconds
    ESP_ERROR_CHECK(esp_timer_start_periodic(update_hw_info_timer_handle, 250 * 1000));
}