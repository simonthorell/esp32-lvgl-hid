cmake_minimum_required(VERSION 3.5)

if (NOT DEFINED PROJECT_NAME)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(esp32-lvgl-hid
else()
    message(FATAL_ERROR "esp32-lvgl-hid: This must be a project's main CMakeLists.txt.")
endif()