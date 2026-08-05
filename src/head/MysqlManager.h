#pragma once
#include "IDataSource.h"
#include "string"
class AppLication;
class MysqlManager : public IDataSource
{
public:
	MysqlManager(AppLication& app);
	~MysqlManager() override = default;
	void Init() override;
	void OnMessage(json& data) override;
public:
	void Send(std::string msg);
	bool InitializeMysql();
	bool IsMySQLRunning(int port, std::string* outInfo, int connectTimeoutMs);
	bool StopMysqldUsingMysqladmin(const std::string& binDir, const std::string& mysqladminArgs, int port, int waitMs);
private:
	AppLication& m_app;
};