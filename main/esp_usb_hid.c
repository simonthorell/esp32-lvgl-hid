#include "esp_usb_hid.h"
#include "gui_textfields.h"

// // https://github.com/espressif/esp-idf/blob/master/examples/peripherals/usb/host/hid/main/hid_host_example.c

// static const char *TAG = "USB_HID_KEYBOARD";

// esp_err_t usb_host_hid_keyboard_init() {
//     // Initialize USB Host (use appropriate USB Host library functions)

//     // Initialize HID driver for keyboards
//     usb_host_driver_t *driver = usb_host_driver_install(USB_HOST_HID);
//     if (!driver) {
//         ESP_LOGE(TAG, "HID driver installation failed");
//         return ESP_FAIL;
//     }

//     // Register HID keyboard event handler
//     usb_host_hid_register_driver(driver, hid_keyboard_event_handler, NULL);

//     // Start USB Host task (use appropriate USB Host library function)
//     usb_host_task();

//     return ESP_OK;
// }

// void hid_keyboard_event_handler(void *arg, usb_host_event_t event, void *event_data) {
//     if (event == USB_HOST_EVENT_KEYBOARD_KEY) {
//         // Handle USB HID keyboard key events here
//         usb_host_keyboard_event_t *key_event = (usb_host_keyboard_event_t *)event_data;
//         char key = key_event->key;
        
//         // Check which LVGL field is currently focused
//         if (lv_obj_has_state(ui_EnterEmailField, LV_STATE_FOCUSED)) {
//             // Update the focused text area with the pressed key
//             if (key >= ' ' && key <= '~') {
//                 char text[2] = {key, '\0'};
//                 lv_textarea_add_text(text_area, text);
//             }
//         } else if (lv_obj_has_state(ui_EnterPasswordField, LV_STATE_FOCUSED)) {
//             // Update the focused input field with the pressed key
//             if (key >= ' ' && key <= '~') {
//                 char text[2] = {key, '\0'};
//                 lv_textarea_add_text(input_field, text);
//             }
//         }
//     }
// }