#pragma once
#include "IDataSource.h"

class RedisManager : public IDataSource
{
public:
	RedisManager();
	~RedisManager() override;
	bool Start() override;
	bool Stop() override;
	bool Init() override;
private:
	std::string zipPath;
	std::string outDir;
};