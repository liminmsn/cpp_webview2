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
private:
	AppLication& m_app;
};