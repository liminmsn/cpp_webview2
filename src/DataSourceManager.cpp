#include "head/DataSourceManager.h"
#include "head/AppLication.h"
#include "head/MysqlManager.h"
#include "head/RedisManager.h"
#include "utils/Path.h"

DataSourceManager::DataSourceManager(AppLication& app) :m_app(app) {
	m_sourcesMap.emplace("MYSQL", std::make_unique<MysqlManager>(m_app));
	m_sourcesMap.emplace("REDIS", std::make_unique<RedisManager>(m_app));
}

void DataSourceManager::OnMessage(json& data) {
	if (data["data"] == "GetState") {
		GetState();
	}
	else if (data["data"] == "InitMysql") {
		m_sourcesMap["MYSQL"]->Init();
	}
	else if (data["data"] == "InitRedis") {
		m_sourcesMap["REDIS"]->Init();
	}
	else if (data["data"]["type"] == "GetOutDir") {
		std::string type = data["data"]["key"].get<std::string>();
		auto iter = m_sourcesMap.find(type);
		if (iter != m_sourcesMap.end())
		{
			auto& dir = m_sourcesMap[type]->outDir;
			m_app.bridge->SendId(dir);
		}
	}
}

void DataSourceManager::GetState() {
	bool mysqlOut = directoryExistsAndNotEmpty(m_sourcesMap["MYSQL"]->outDir);
	bool redisOut = directoryExistsAndNotEmpty(m_sourcesMap["REDIS"]->outDir);
	m_app.bridge->SendId(
		{
			{
				"mysql",
				{
					{"InitialState",mysqlOut},
					{"outDir",m_sourcesMap["MYSQL"]->outDir}
				}
			},
			{
				"redis",
				{
					{"InitialState",redisOut},
					{"outDir",m_sourcesMap["REDIS"]->outDir}
				}
			}
		});
}