#pragma once
#include "../utils/JsonUtil.h"
#include "../utils/Encoding.h"
#include "IDataSource.h"
#include <memory>
#include <string>

class AppLication;
class DataSourceManager
{
public:
	explicit DataSourceManager(AppLication& app);
	void OnMessage(json& data);
private:
	AppLication& m_app;
	std::unordered_map<std::string, std::unique_ptr<IDataSource>> m_sourcesMap;
	void GetState();
};