#include "head/DataSourceManager.h"
#include "head/AppLication.h"
#include "head/MysqlManager.h"
#include "head/RedisManager.h"
#include "utils/Path.h"
#include "utils/File.h"

DataSourceManager::DataSourceManager(AppLication& app) :m_app(app) {
	m_sourcesMap.emplace("MYSQL", std::make_unique<MysqlManager>(m_app));
	m_sourcesMap.emplace("REDIS", std::make_unique<RedisManager>(m_app));
}

void DataSourceManager::OnMessage(json& pos) {
	if (pos["data"] == "GetState") {
		GetState();
	}
	else if (pos["data"] == "InitMysql") {
		m_sourcesMap["MYSQL"]->Init();
	}
	else if (pos["data"] == "InitRedis") {
		m_sourcesMap["REDIS"]->Init();
	}
	else if (pos["data"]["type"] == "GetOutDir") {
		std::string type = pos["data"]["key"].get<std::string>();
		this->GetOutDir(type);
	}
	else if (pos["data"]["type"] == "FileExists") {
		std::string& path = pos["data"]["path"].get<std::string>();
		m_app.bridge->SendId(FileExists(path));
	}
	else if (pos["data"]["type"] == "MYSQL") {
		m_sourcesMap["MYSQL"]->OnMessage(pos["data"]["data"]);
	}
	else if (pos["data"]["type"] == "REDIS") {
		m_sourcesMap["REDIS"]->OnMessage(pos["data"]["data"]);
	}
}

void DataSourceManager::GetOutDir(std::string type) {
	auto iter = m_sourcesMap.find(type);
	if (iter != m_sourcesMap.end())
	{
		auto& dir = m_sourcesMap[type]->outDir;
		m_app.bridge->SendId(dir);
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
					{"outDir",m_sourcesMap["MYSQL"]->outDir},
					{"Initd",FileExists(m_sourcesMap["MYSQL"]->outDir + "\\my.ini")}
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