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
private:
	PROCESS_INFORMATION* outPi = nullptr;
	AppLication& m_app;
};