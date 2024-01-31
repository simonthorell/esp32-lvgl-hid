file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(
        SRCS "main.c" "connect_wifi.c" "connect_mqtt.c" "gui_buttons.c" "gui_textfields.c" 
             "esp_hardware.c" "esp_usb_hid.c" "esp_firmware.c" ${SRC_UI}
        INCLUDE_DIRS "." "ui" "include"
        PRIV_REQUIRES tdisplays3
        REQUIRES nvs_flash mqtt esp_wifi esp_http_client esp_https_ota json usb esp_driver_gpio)
        # usb and esp_driver_gpio may be required for USB HID

### FOTA - Firmware Over The Air ###
# Copy the binary file to 'bin' directory and rename it to firmware.bin
add_custom_target(copy_firmware ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/check_and_copy.cmake" "${CMAKE_SOURCE_DIR}/build/esp32-lvgl-hid.bin" "${CMAKE_SOURCE_DIR}/bin/firmware.bin"
    COMMENT "Conditionally copying esp32-lvgl-hid.bin to bin/firmware.bin if it exists"
)
# add_custom_target(copy_firmware ALL
#         COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/bin"
#         COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/esp32-lvgl-hid.bin" "${CMAKE_SOURCE_DIR}/bin/firmware.bin"
#         COMMENT "Copying esp32-lvgl-hid.bin to bin/firmware.bin"
# )