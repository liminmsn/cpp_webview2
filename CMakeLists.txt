cmake_minimum_required(VERSION 3.10.0)
project(cpp_webview2 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WIN32_EXECUTABLE TRUE)
add_definitions("/utf-8")


# vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(cpp_webview2
    main.cpp
    resource.rc
    ${SRC_FILES}
)

set_target_properties(cpp_webview2 PROPERTIES
    OUTPUT_NAME "好看韩剧5"
)

