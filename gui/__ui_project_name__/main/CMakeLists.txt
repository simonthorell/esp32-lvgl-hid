file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")

idf_component_register(SRCS "main.c" "connect_wifi.c" "connect_mqtt.c" ${SRC_UI}
        INCLUDE_DIRS "." "ui"
        PRIV_REQUIRES tdisplays3
        REQUIRES nvs_flash mqtt esp_wifi)