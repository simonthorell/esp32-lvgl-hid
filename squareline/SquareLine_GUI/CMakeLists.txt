cmake_minimum_required(VERSION 3.5)

if (NOT DEFINED PROJECT_NAME)
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(SquareLine_GUI)
else()
    message(FATAL_ERROR "SquareLine_GUI: This must be a project's main CMakeLists.txt.")
endif()