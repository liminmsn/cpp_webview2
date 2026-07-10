# cpp_webview2

## 项目简介

`cpp_webview2` 是一个基于 Win32 的 C++ 示例项目，使用 `CMake` 和 `vcpkg` 进行构建管理。项目当前包含一个简单的 Windows 桌面程序，用于创建基本窗口并展示默认消息循环。

该项目已配置 `unofficial-webview2`、`cpr` 和 `nlohmann-json` 依赖，适合后续扩展为 WebView2 界面或网络交互应用。

## 主要内容

- `main.cpp`：项目入口，初始化 Win32 窗口类并启动应用消息循环。
- `resource.rc` / `resource.h`：窗口资源和图标定义。
- `CMakeLists.txt`：CMake 构建脚本，设置 C++17、vcpkg 工具链并生成可执行文件 `cpp_webview2`。
- `vcpkg.json`：vcpkg 依赖列表。
- `CMakePresets.json`：Visual Studio 配置预设。
- `src/`：项目源码目录，当前含示例 `Test.cpp` 和头文件目录 `head/`。

## 依赖项

项目依赖由 `vcpkg.json` 定义：

- `cpr`
- `nlohmann-json`
- `unofficial-webview2`

## 编译环境

推荐使用 Windows 平台下的 Visual Studio 2026 或更高版本，并通过 `vcpkg` 提供依赖。CMake 最低版本要求为 3.10。

## 构建说明

1. 安装并引导 `vcpkg`
   - `vcpkg/bootstrap-vcpkg.bat`

2. 安装依赖
   - `vcpkg/vcpkg.exe install --triplet x64-windows cpr nlohmann-json unofficial-webview2`

3. 配置并生成工程
   - 使用 Visual Studio 自带 CMake 支持，或命令行执行：
     ```powershell
     cmake -S . -B out/build/Visual Studio Community 2026 Release - amd64 -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
     ```

4. 构建工程
   - 在 Visual Studio 中打开生成的解决方案并编译，或命令行执行：
     ```powershell
     cmake --build out/build/Visual Studio Community 2026 Release - amd64 --config Debug
     ```
     > 这里的配置名称可以根据 `CMakePresets.json` 中的 preset 进行调整。

## 运行

构建成功后，生成的可执行文件位于 `out/build/Visual Studio Community 2026 Release - amd64/Debug/`（或对应安装路径）下。

## 项目结构

```
cpp_webview2/
├── CMakeLists.txt
├── CMakePresets.json
├── main.cpp
├── resource.h
├── resource.rc
├── src/
│   ├── Test.cpp
│   └── head/
│       └── Test.h
├── vcpkg.json
└── vcpkg/
```

## 备注

- 当前项目入口为 Win32 原生窗口示例，尚未在 `main.cpp` 中实际创建 WebView2 控件。
- 后续可在 `main.cpp` 中集成 `unofficial-webview2` 并使用 `cpr` / `nlohmann-json` 实现网络请求与 JSON 解析。