#pragma once
#include "IDataSource.h"

class AppLication;
class RedisManager : public IDataSource
{
public:
	RedisManager(AppLication& app);
	~RedisManager() override = default;
	void Init() override;
	void OnMessage(json& data) override;
	void Send(std::string msg);
	bool IsRedisProcessRunning();
private:
	AppLication& m_app;
};