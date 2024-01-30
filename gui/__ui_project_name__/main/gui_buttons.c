#include "gui_buttons.h"
#include "app_common.h"
#include <esp_log.h>

// GPIO pin numbers for the buttons
gpio_num_t btn_gpio_nums[2] = {
        BTN_PIN_NUM_1,
        BTN_PIN_NUM_2,
};

// store button handles
button_handle_t btn_handles[2];
// keep a track of button press states
bool btn_1_pressed = false;
bool btn_2_pressed = false;
// Define Clear Textfields (hold for 1 second)
lv_timer_t* hold_timer = NULL;
bool is_btn_1_held = false;
bool is_btn_2_held = false;

// Function to configure the boo & GPIO14 buttons using espressif/button component
void setup_buttons() {
    for (size_t i = 0; i < 2; i++) {
        ESP_LOGI(TAG, "Configuring button %ld", ((int32_t) i) + 1);
        button_config_t gpio_btn_cfg = {
                .type = BUTTON_TYPE_GPIO,
                .short_press_time = 500,
                .gpio_button_config = {
                        .gpio_num = (int32_t) btn_gpio_nums[i],
                        .active_level = 0,
                },
        };
        button_handle_t btn_handle = iot_button_create(&gpio_btn_cfg);
        if (NULL == btn_handle) {
            ESP_LOGE(TAG, "Button %d create failed", i + 1);
        }
        esp_err_t err = iot_button_register_cb(btn_handle, BUTTON_PRESS_DOWN, button_press_down_cb, NULL);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "error iot_button_register_cb [button %d]: %s", i + 1, esp_err_to_name(err));
        }
        err = iot_button_register_cb(btn_handle, BUTTON_PRESS_UP, button_press_up_cb, NULL);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "error iot_button_register_cb [button %d]: %s", i + 1, esp_err_to_name(err));
        }
        btn_handles[i] = btn_handle;
    }
}

// Callback function when buttons are pressed down
void button_press_down_cb(void *arg, void *usr_data) {
    button_handle_t button = (button_handle_t) arg;

    // Start a timer for hold button time (clear field)
    if (hold_timer == NULL) {
        hold_timer = lv_timer_create(clear_textarea, HOLD_TIME_MS, usr_data);
    }

    if (button == btn_handles[0]) {
        // ESP_LOGI(TAG, "0 BUTTON_PRESS_DOWN");
        btn_1_pressed = true;
        is_btn_1_held = true; // Track if the button is held

        lv_textarea_set_cursor_pos(ui_EnterEmailField, LV_TEXTAREA_CURSOR_LAST); // set cursor to start of Email field
        lv_textarea_add_text(ui_EnterEmailField, "first.lastname@emailaddress.com");
        lv_obj_add_style(ui_EnterEmailField, &style_ta_focused, LV_PART_MAIN); // focus
        lv_obj_add_style(ui_EnterPasswordField, &style_ta_unfocused, LV_PART_MAIN); // unfocus
    }
    if (button == btn_handles[1]) {
        // ESP_LOGI(TAG, "1 BUTTON_PRESS_DOWN");
        btn_2_pressed = true;
        is_btn_2_held = true; // Track if the button is held

        lv_textarea_set_cursor_pos(ui_EnterPasswordField, LV_TEXTAREA_CURSOR_LAST); // set cursor to start of Password field
        lv_textarea_add_text(ui_EnterPasswordField, "mysecretpassword123");
        lv_obj_add_style(ui_EnterPasswordField, &style_ta_focused, LV_PART_MAIN); // focus
        lv_obj_add_style(ui_EnterEmailField, &style_ta_unfocused, LV_PART_MAIN); // unfocus
    }

    if (btn_1_pressed && btn_2_pressed) {
        // Placeholder function for sending email and password over MQTT
        send_credentials_over_mqtt(ui_EnterEmailField, ui_EnterPasswordField);

        // Clear the input fields & unfocus
        lv_textarea_set_text(ui_EnterEmailField, "");
        lv_textarea_set_text(ui_EnterPasswordField, "");
        lv_obj_add_style(ui_EnterEmailField, &style_ta_unfocused, LV_PART_MAIN);
        lv_obj_add_style(ui_EnterPasswordField, &style_ta_unfocused, LV_PART_MAIN);

        // Change the label's text and color on submit
        original_text_color = lv_obj_get_style_text_color(ui_SubmitLabel, LV_PART_MAIN); // Save the original color

        lv_label_set_text(ui_SubmitLabel, "Submitted to Bosse!");
        lv_obj_set_style_text_color(ui_SubmitLabel, lv_color_make(0, 255, 0), LV_PART_MAIN);

        // Start a timer to revert the changes after 2 seconds
        if (revert_timer == NULL) {
            revert_timer = lv_timer_create(revert_label_style, 1500, NULL);
        }
    }
}

// Callback function when buttons are released
void button_press_up_cb(void *arg, void *usr_data) {
    button_handle_t button = (button_handle_t) arg;

    // Reset the flag as the button is released
    is_btn_1_held = false;
    is_btn_2_held = false;

    // If the timer is running but the button is released before 2 seconds, stop and delete the timer
    if (hold_timer != NULL) {
        lv_timer_del(hold_timer);
        hold_timer = NULL;
    }

    if (button == btn_handles[0]) {
//        ESP_LOGI(TAG, "0 BUTTON_PRESS_UP");
        btn_1_pressed = false;
    }
    if (button == btn_handles[1]) {
//        ESP_LOGI(TAG, "1 BUTTON_PRESS_UP");
        btn_2_pressed = false;
    }
}
