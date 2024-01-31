file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(SRCS "main.c" "connect_wifi.c" "connect_mqtt.c" "gui_buttons.c" "gui_textfields.c" "esp_hardware.c" "esp_usb_hid.c" ${SRC_UI}
        INCLUDE_DIRS "." "ui" "include"
        PRIV_REQUIRES tdisplays3
        REQUIRES nvs_flash mqtt esp_wifi usb esp_driver_gpio)
        # usb and esp_driver_gpio may be required for USB HID