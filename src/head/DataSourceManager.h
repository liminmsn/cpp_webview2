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
	~DataSourceManager();
	void OnMessage(json &data);
	void InitAll();
	void StartAll();
	void StopAll();
private:
	AppLication& m_app;
	std::vector<std::unique_ptr<IDataSource>> m_sources;
};