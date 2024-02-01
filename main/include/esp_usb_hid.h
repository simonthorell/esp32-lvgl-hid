#ifndef ESP_USB_HID_H
#define ESP_USB_HID_H

#include "app_common.h"

#include <stdint.h>
#include "usb/hid_host.h"
#include "usb/hid_usage_keyboard.h"
#include "usb/hid_usage_mouse.h"

// Helper Functions
void hid_host_interface_callback(hid_host_device_handle_t hid_device_handle,
                                 const hid_host_interface_event_t event,
                                 void *arg);
void hid_host_device_event(hid_host_device_handle_t hid_device_handle,
                           const hid_host_driver_event_t event,
                           void *arg);
void hid_host_device_callback(hid_host_device_handle_t hid_device_handle,
                              const hid_host_driver_event_t event,
                              void *arg);

// Core Function
void esp_usb_hid_init();

#endif // ESP_USB_HID_H