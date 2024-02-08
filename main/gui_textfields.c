#include "gui_textfields.h"
#include "app_common.h"
#include <esp_log.h>

#define TAG "__UI_PROJECT_NAME__"

// Define textfield styles
lv_style_t style_ta_focused;
lv_style_t style_ta_unfocused;

// Define other global variables related to the textfield
lv_color_t original_text_color;
const char* original_text = "Submit to Bosse pressing both buttons";
lv_timer_t* revert_timer = NULL;

// Setup style for input fields
void setup_inputfields() {
    // Set original status text/label color
    original_text_color = lv_color_make(200, 200, 200);

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
void clear_textarea(lv_timer_t* timer) {

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

void revert_label_style(lv_timer_t* timer) {
    // Revert the label's text and color
    lv_label_set_text(ui_SubmitLabel, original_text);
    lv_obj_set_style_text_color(ui_SubmitLabel, original_text_color, LV_PART_MAIN);

    // Delete the timer as its job is done
    lv_timer_del(timer);
    revert_timer = NULL;
}

void update_wifi_status_ui(bool connected) {
    if (connected) {
        lv_label_set_text(ui_SubmitLabel, "Connected");
        lv_obj_set_style_text_color(ui_SubmitLabel, lv_color_make(0, 255, 0), LV_PART_MAIN); // Green color

        // Start a timer to revert the changes after 2 seconds
        if (revert_timer == NULL) {
            revert_timer = lv_timer_create(revert_label_style, 2500, NULL);
        }

    } else {
        lv_label_set_text(ui_SubmitLabel, "Disconnected");
        lv_obj_set_style_text_color(ui_SubmitLabel, lv_color_make(255, 0, 0), LV_PART_MAIN); // Red color
    }
}


void firmware_version_ui(double FIRMWARE_VERSION) {
    char buffer[32]; // Buffer to hold the formatted string, adjust size as needed
    sprintf(buffer, "Firmware version: %.2f", FIRMWARE_VERSION); // %.2f limits the double to two decimal places

    lv_label_set_text(ui_SubmitLabel, buffer);
    lv_obj_set_style_text_color(ui_SubmitLabel, lv_color_make(0, 0, 255), LV_PART_MAIN); // Blue color
}