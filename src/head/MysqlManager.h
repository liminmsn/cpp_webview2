#pragma once
#include "IDataSource.h"
#include "string"
class AppLication;
class MysqlManager : public IDataSource
{
public:
	MysqlManager(AppLication& app);
	~MysqlManager() override;
	bool Start() override;
	bool Stop() override;
	bool Init() override;
	AppLication& m_app;
private:
	std::string zipPath;
	std::string outDir;
};