#pragma once
#include "IDataSource.h"

class AppLication;
class RedisManager : public IDataSource
{
public:
	RedisManager(AppLication& app);
	~RedisManager() override = default;
	void Init() override;
private:
	AppLication& m_app;
};