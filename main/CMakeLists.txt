file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(SRCS "main.c" "connect_wifi.c" "connect_mqtt.c" "gui_buttons.c" "gui_textfields.c" "esp_hardware.c" "esp_usb_hid.c" ${SRC_UI}
        INCLUDE_DIRS "." "ui" "include"
        PRIV_REQUIRES tdisplays3
        REQUIRES nvs_flash mqtt esp_wifi usb esp_driver_gpio)
        # usb and esp_driver_gpio may be required for USB HID

# Copy the binary file to 'bin' directory and rename it to firmware.bin
add_custom_target(copy_firmware ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/esp32-lvgl-hid.bin" "${CMAKE_SOURCE_DIR}/bin/firmware.bin"
        COMMENT "Copying esp32-lvgl-hid.bin to bin/firmware.bin"
        )