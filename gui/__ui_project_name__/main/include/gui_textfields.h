#ifndef GUI_TEXTFIELDS_H
#define GUI_TEXTFIELDS_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ui/ui.h"
#include "gui_buttons.h" // Include this if you need button-related variables or functions

// Declare textfield styles
extern lv_style_t style_ta_focused;
extern lv_style_t style_ta_unfocused;

// Declare other global variables related to the textfield
extern lv_color_t original_text_color;
extern const char* original_text;
extern lv_timer_t* revert_timer;

// Function declarations
void setup_inputfields();
void clear_textarea(lv_timer_t* timer);
void revert_label_style(lv_timer_t* timer);
void update_wifi_status_ui(bool connected);

#endif // GUI_TEXTFIELDS_H