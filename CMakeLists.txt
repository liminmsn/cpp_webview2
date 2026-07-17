cmake_minimum_required(VERSION 3.16)

project(cpp_webview2
    VERSION 0.1.0
    LANGUAGES C CXX
)

# C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Win32 GUI
set(CMAKE_WIN32_EXECUTABLE TRUE)

# UTF-8 编译
add_compile_options("/utf-8")

# vcpkg 包
find_package(unofficial-webview2 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

# 自动收集源码
file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

# 可执行程序
add_executable(cpp_webview2
    main.cpp
    resource.rc
    ${SRC_FILES}
)

# 输出名称
set_target_properties(cpp_webview2 PROPERTIES
    OUTPUT_NAME "好看韩剧5"
)

# 链接库
target_link_libraries(cpp_webview2 PRIVATE
    unofficial::webview2::webview2
    nlohmann_json::nlohmann_json
    cpr::cpr              # ✅ 注意这里要写 cpr::cpr，而不是 cpr
    dcomp                 # DirectComposition
    dwmapi                # DWM API
)

# Windows SDK 依赖（可选，确保链接到系统库）
target_link_libraries(cpp_webview2 PRIVATE
    user32
    gdi32
    ole32
    shell32
)
