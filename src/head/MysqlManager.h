#pragma once

#include <filesystem>
#include <string>

class Application;

class MysqlManager
{
public:
	explicit MysqlManager(const Application& app);
	~MysqlManager();

	// 初始化（读取路径、检查环境）
	bool Init();

	// 安装
	bool Install();          // 解压 mysql
	bool Initialize();       // 初始化 data 目录
	bool CreateConfig();     // 生成 my.ini
	bool Uninstall();        // 卸载

	// 运行
	bool Start();
	bool Stop();
	bool Restart();

	// Windows 服务
	bool InstallService();
	bool IsInstallService();
	bool RemoveService();
	bool StartService();
	bool StopService();

	// 状态
	bool IsInstalled() const;
	bool IsInitialized() const;
	bool IsRunning() const;

	// SQL
	bool Connect(
		const std::string& host,
		int port,
		const std::string& user,
		const std::string& password);

	void Disconnect();

	std::string Execute(const std::string& sql);

	// 路径
	const std::filesystem::path& GetRootPath() const;
	const std::filesystem::path& GetMysqlPath() const;
	const std::filesystem::path& GetDataPath() const;
	const std::filesystem::path& GetConfigPath() const;

private:
	bool ExtractZip();
	bool RunCommand(const std::wstring& command);

private:
	const Application& m_app;

	std::filesystem::path exePath_;
	std::filesystem::path resourcePath_;

	std::filesystem::path rootPath_;
	std::filesystem::path mysqlPath_;
	std::filesystem::path dataPath_;
	std::filesystem::path configPath_;

	bool installed_{ false };
	bool initialized_{ false };
	bool running_{ false };
};