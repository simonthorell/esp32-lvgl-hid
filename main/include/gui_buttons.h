#ifndef GUI_BUTTONS_H
#define GUI_BUTTONS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "t_display_s3.h" // for BTN_PIN_NUM_1, BTN_PIN_NUM_2
#include "ui/ui.h"
#include "iot_button.h"
#include "gui_textfields.h"

#define HOLD_TIME_MS 1000 // 1 second

extern gpio_num_t btn_gpio_nums[2];
extern button_handle_t btn_handles[2];

// Declare global variables related to button press functionality
extern bool btn_1_pressed;
extern bool btn_2_pressed;

// Declare global variables related to button hold functionality
extern lv_timer_t* hold_timer;
extern bool is_btn_1_held;
extern bool is_btn_2_held;

void setup_buttons(void);
void button_press_down_cb(void *arg, void *usr_data);
void button_press_up_cb(void *arg, void *usr_data);

#endif // GUI_BUTTONS_H