#pragma once

#include <string>
#include <iostream>
#include <filesystem>

class Application;
class MysqlManager
{
public:
    MysqlManager(const Application& app);
    ~MysqlManager();

    // 初始化（检查目录、创建配置等）
    bool Init();

    // 是否已安装
    bool IsInstalled() const;

    // 安装（解压 ZIP）
    bool Install();

    // 初始化数据库
    bool Initialize();

    // 启动 / 停止
    bool Start();
    bool Stop();
    bool Restart();

    // 状态
    bool IsRunning() const;

    // SQL（以后用 Connector/C++ 实现）
    bool Connect(
        const std::string& host,
        int port,
        const std::string& user,
        const std::string& password);

    bool Disconnect();

    std::string Execute(const std::string& sql);

    // 路径
    const std::filesystem::path& GetRootPath() const;
    const std::filesystem::path& GetMysqlPath() const;
    const std::filesystem::path& GetDataPath() const;
    const std::filesystem::path& GetConfigPath() const;

private:
    const Application& m_app;
    std::filesystem::path exePath_;
    std::filesystem::path resourcePath_;
    std::filesystem::path rootPath_;
    std::filesystem::path mysqlPath_;
    std::filesystem::path dataPath_;
    std::filesystem::path configPath_;

    bool installed_{ false };
    bool running_{ false };
    bool ExtractZip() const;
    bool CreateConfig() const;
};