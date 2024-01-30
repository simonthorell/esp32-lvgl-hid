#ifndef ESP_HARDWARE_H
#define ESP_HARDWARE_H

#include <math.h> // ceil
#include <esp_log.h>
#include "esp_timer.h" 
#include "t_display_s3.h" // For display_s3_init()
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui/ui.h"
#include "app_common.h" // For TAG

// Declare battery status variables
extern int battery_voltage;
extern int battery_percentage;
extern bool on_usb_power;

// Declare LCD Screen brightness variables
extern int last_screen_brightness_step;
extern int screen_brightness_step;
extern int current_battery_symbol_idx;

// Function declarations
void lcd_fade_in(void);
void configure_hardware_timer(void);
void update_hw_info_timer_cb(void *arg);
void update_ui(void);
void ui_update_task(void *pvParam);

#endif // ESP_HARDWARE_H
