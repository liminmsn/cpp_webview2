# cpp_webview2

项目说明（简短）

cpp_webview2 是一个基于 Win32 的 C++ 桌面示例工程，使用 CMake（Visual Studio 18 2026 生成器）和 vcpkg 管理依赖。项目演示了基础窗口应用、资源自动解压流程以及简单的数据源管理（MySQL/Redis 资源解压与状态检测），并为后续集成 WebView2、网络请求和 JSON 处理提供基础设施。

主要特性

- 使用 CMake + vcpkg 构建，目标平台 Windows（MSVC，C++17）。
- 管理资源包（resources/zip）并自动解压到 resources/mysql、resources/redis（由 MysqlManager/RedisManager 控制）。
- DataSource 管理器（DataSourceManager）用于统一初始化并上报状态到应用桥接（bridge）。
- 使用 nlohmann-json、cpr、unofficial-webview2（可选，见 vcpkg.json）作为潜在扩展依赖。

依赖项

- vcpkg 管理，示例依赖列在 vcpkg.json：cpr、nlohmann-json、unofficial-webview2（按需安装）。

构建要求

- Windows + Visual Studio 2026（或兼容 MSVC）
- CMake 最低版本 3.16
- vcpkg（可选，但推荐用于一致的依赖管理）

构建步骤（命令行示例）

1. 引导 vcpkg（如果使用）：
   - `vcpkg\bootstrap-vcpkg.bat`

2. 安装依赖（可选）：
   - `vcpkg\vcpkg.exe install --triplet x64-windows cpr nlohmann-json unofficial-webview2`

3. 生成工程：
   - `cmake -S . -B "out/build/Visual Studio Community 2026 Release - amd64" -G "Visual Studio 18 2026" -A x64 -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake`

4. 编译：
   - `cmake --build "out/build/Visual Studio Community 2026 Release - amd64" --config Debug`

运行

构建输出位于 `out/build/Visual Studio Community 2026 Release - amd64/{Debug|Release}` 下，运行可执行文件以启动应用。

资源与初始化（DataSource 说明）

- `resources/zip`：存放待解压的资源包（例如 mysql-8.4.10-winx64.zip、Redis-*.zip）。
- `MysqlManager` / `RedisManager`：构造时计算安装路径（`GetInstallPath`），并将 `zipPath/outDir` 设置到 `resources` 下的 `mysql/redis`。`Init()` 会在目录为空时异步解压并通过应用桥（bridge）上报解压进度与结果。
- `DataSourceManager::GetState`：通过 `directoryExistsAndNotEmpty(outDir)` 判断 `InitialState`，含义为“目录存在且非空（已初始化/已解压）”。如果需区分“目录不存在”与“存在但为空”，可扩展为返回额外字段（`exists`、`notEmpty`）。

项目结构（主要文件）

```
cpp_webview2/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── main.cpp
├── src/
│   ├── head/                # 头文件（AppLication、IDataSource、MysqlManager、RedisManager 等）
│   ├── manager/             # 管理器实现（MysqlManager.cpp、RedisManager.cpp）
│   ├── utils/               # 工具（Path.h、Zip.h、Encoding 等）
│   └── DataSourceManager.cpp
├── resources/
│   ├── zip/                 # 内置 zip 包
│   ├── mysql/               # 解压后 MySQL 资源
│   └── redis/               # 解压后 Redis 资源
└── out/                     # CMake 构建输出目录
```

调试与验证

- 构建并运行后，可在构建输出的 `resources` 目录下检查 `mysql/redis` 是否已解压。
- 触发 `DataSourceManager` 的 `GetState`（应用通过 bridge 发送消息），返回的 `InitialState` 表示资源目录是否存在且非空。

扩展建议（短）

- 集成 WebView2 控件以实现前端 UI（unofficial-webview2）。
- 使用 cpr + nlohmann-json 实现应用内网络请求与数据处理。
- 将资源状态扩展为更细粒度的枚举或结构以便前端展示（例如：NotExists / Empty / Ready / Extracting / Error）。

许可与贡献

按仓库已有约定贡献代码，提交前请保持代码风格一致并通过本地构建测试.