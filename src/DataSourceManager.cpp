#include "head/DataSourceManager.h"
#include "head/AppLication.h"
#include "head/MysqlManager.h"
#include "head/RedisManager.h"
#include "utils/Path.h"

DataSourceManager::DataSourceManager(AppLication& app) :m_app(app) {
	m_sources.emplace_back(std::make_unique<MysqlManager>(m_app));
	m_sources.emplace_back(std::make_unique<RedisManager>(m_app));
};

void DataSourceManager::OnMessage(json& data) {
	if (data["data"] == "GetState") {
		GetState();
	}
	else if (data["data"] == "InitMysql") {
		m_sources[0]->Init();
	}
	else if (data["data"] == "InitRedis") {
		m_sources[1]->Init();
	}
}

void DataSourceManager::GetState() {
	bool mysqlOut = directoryExistsAndNotEmpty(m_sources[0]->outDir);
	bool redisOut = directoryExistsAndNotEmpty(m_sources[1]->outDir);
	m_app.bridge->SendId(
		{
			{
				"mysql",
				{
					{"InitialState",mysqlOut}
				}
			},
			{
				"redis",
				{
					{"InitialState",redisOut}
				}
			}
		});
}