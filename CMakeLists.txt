cmake_minimum_required(VERSION 3.31)

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
find_package(miniz CONFIG REQUIRED)

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
    OUTPUT_NAME "LocalMysql"
)

# 链接库
target_link_libraries(cpp_webview2 PRIVATE
    unofficial::webview2::webview2
    nlohmann_json::nlohmann_json
    miniz::miniz
    cpr::cpr
    dcomp
    dwmapi
    user32
    gdi32
    ole32
    oleaut32
    shell32
)

# 编译完成后复制资源
add_custom_command(TARGET cpp_webview2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:cpp_webview2>/resources"
)

# Debug / Release 宏
target_compile_definitions(cpp_webview2 PRIVATE
    $<$<CONFIG:Debug>:WEBVIEW_DEBUG>
    $<$<CONFIG:Release>:WEBVIEW_RELEASE>
)

############################################################
# Install
############################################################

include(GNUInstallDirs)

# 安装 EXE
install(TARGETS cpp_webview2
    RUNTIME DESTINATION .
)

# 安装 Release 目录中的所有 DLL
install(DIRECTORY
    "$<TARGET_FILE_DIR:cpp_webview2>/"
    DESTINATION .
    FILES_MATCHING
    PATTERN "*.dll"
)

# 安装 resources
install(DIRECTORY
    "${CMAKE_SOURCE_DIR}/resources/"
    DESTINATION resources
)