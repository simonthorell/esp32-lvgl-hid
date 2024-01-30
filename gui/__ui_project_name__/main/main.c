#include <stdio.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <math.h> // ceil
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "t_display_s3.h"
#include "iot_button.h"
#include "ui/ui.h"
#include "mqtt_client.h"
#include "connect_wifi.h"
#include "connect_mqtt.h"

#define TAG "__UI_PROJECT_NAME__"

// GPIO pin numbers for the buttons
static gpio_num_t btn_gpio_nums[2] = {
        BTN_PIN_NUM_1,
        BTN_PIN_NUM_2,
};

// store button handles
button_handle_t btn_handles[2];

// keep a track of button press states
bool btn_1_pressed = false;
bool btn_2_pressed = false;

// Battery status variables
int battery_voltage;       // Currently unused
int battery_percentage;    // Currently unused
bool on_usb_power = false; // Currently unused

// LCD Screen brightness variables
int last_screen_brightness_step = 16;
int screen_brightness_step = 16;
int current_battery_symbol_idx = 0;

// Define textfield styles
static lv_style_t style_ta_focused;
static lv_style_t style_ta_unfocused;

// Define Clear Textfields (hold for 1 second)
static lv_timer_t* hold_timer = NULL;
static bool is_btn_1_held = false;
static bool is_btn_2_held = false;
static const uint32_t HOLD_TIME_MS = 1000; // 1 seconds

// Submit label (change on submit for 1 second)
static lv_color_t original_text_color;    // Store the original text color
static const char* original_text = "Submit to Bosse pressing both buttons";
static lv_timer_t* revert_timer = NULL;

// Setup style for input fields
void setup_inputfields() {
    lv_textarea_set_password_mode(ui_EnterPasswordField, true);
    lv_textarea_set_max_length(ui_EnterEmailField, 35);
    lv_textarea_set_max_length(ui_EnterPasswordField, 35);

    lv_style_init(&style_ta_focused);
    lv_style_set_border_color(&style_ta_focused, lv_color_make(0x55, 0x96, 0xd8)); // Example color
    lv_style_set_border_width(&style_ta_focused, 2);

    lv_style_init(&style_ta_unfocused);
    lv_style_set_border_color(&style_ta_unfocused, lv_color_make(0x88, 0x88, 0x88)); // Example color
    lv_style_set_border_width(&style_ta_unfocused, 1);
}

// Clear the textfield when button is held for 2 seconds
static void clear_textarea(lv_timer_t* timer) {

    if (is_btn_1_held) {
        lv_textarea_set_text(ui_EnterEmailField, "");
    }

    if (is_btn_2_held) {
        lv_textarea_set_text(ui_EnterPasswordField, "");
    }

    // Stop and delete the timer as its job is done
    lv_timer_del(timer);
    hold_timer = NULL;
}

static void revert_label_style(lv_timer_t* timer) {
    // Revert the label's text and color
    lv_label_set_text(ui_SubmitLabel, original_text);
    lv_obj_set_style_text_color(ui_SubmitLabel, original_text_color, LV_PART_MAIN);

    // Delete the timer as its job is done
    lv_timer_del(timer);
    revert_timer = NULL;
}

// Placeholder function definition, needs to be implemented
void send_credentials_over_mqtt(lv_obj_t* emailField, lv_obj_t* passwordField) {
    const char* email = lv_textarea_get_text(emailField);
    const char* password = lv_textarea_get_text(passwordField);

    // TODO: Implement MQTT sending logic here
    ESP_LOGI(TAG, "Sending credentials over MQTT - Email: %s, Password: %s", email, password);
}

// Callback function when buttons are pressed down
static void button_press_down_cb(void *arg, void *usr_data) {
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
static void button_press_up_cb(void *arg, void *usr_data) {
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

// Function to configure the boo & GPIO14 buttons using espressif/button component
static void setup_buttons() {
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


static void update_hw_info_timer_cb(void *arg) {
    battery_voltage = get_battery_voltage();
    on_usb_power = usb_power_voltage(battery_voltage);
    battery_percentage = (int) volts_to_percentage((double) battery_voltage / 1000);
}

static void update_ui() {
    // Perform your ui updates here
    ESP_LOGI(TAG, "update_ui function call");
}

static void ui_update_task(void *pvParam) {
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

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    ESP_LOGI("CH", "%d ret", ret);
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

    connect_wifi(); // Connect to wifi

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