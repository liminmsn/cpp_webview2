cmake_minimum_required(VERSION 3.10)

project(utils LANGUAGES C)

add_library(utils STATIC
    utils.c
 "utils.h")

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)