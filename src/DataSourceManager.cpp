#include "head/DataSourceManager.h"
#include "head/AppLication.h"
#include "head/MysqlManager.h"
#include "head/RedisManager.h"
#include "utils/Path.h"
#include "utils/File.h"
#include <utils.h>

DataSourceManager::DataSourceManager(AppLication& app) :m_app(app) {
	m_sourcesMap.emplace("MYSQL", std::make_unique<MysqlManager>(m_app));
	m_sourcesMap.emplace("REDIS", std::make_unique<RedisManager>(m_app));
}

void DataSourceManager::OnMessage(json& pos) {
	if (pos["data"] == "GetState")
		GetState();
	else if (pos["data"] == "OpenWithExplorer")
		m_app.bridge->SendId(OpenWithExplorer(pos["path"]));
	else if (pos["data"] == "OpenNewTerminal") {
		std::string pathUtf8 = pos["path"].get<std::string>();
		std::string argsUtf8 = pos["args"].get<std::string>();
		m_app.bridge->SendId(
			(bool)utils.open_mysql_terminal(
				Utf8ToUtf16(pathUtf8).c_str(), 
				argsUtf8.c_str()
			)
		);
	}
	else if (pos["data"] == "InitMysql")
			m_sourcesMap["MYSQL"]->Init();
	else if (pos["data"] == "InitRedis")
			m_sourcesMap["REDIS"]->Init();
	else if (pos["data"]["type"] == "GetOutDir")
			GetOutDir(pos["data"]["key"].get<std::string>());
	else if (pos["data"]["type"] == "MYSQL")
			m_sourcesMap["MYSQL"]->OnMessage(pos["data"]["data"]);
	else if (pos["data"]["type"] == "REDIS")
			m_sourcesMap["REDIS"]->OnMessage(pos["data"]["data"]);
	else if (pos["data"]["type"] == "FileExists") {
		std::string& path = pos["data"]["path"].get<std::string>();
		m_app.bridge->SendId(FileExists(path));
	}
}

void DataSourceManager::GetOutDir(std::string type) {
	auto iter = m_sourcesMap.find(type);
	if (iter != m_sourcesMap.end())
		m_app.bridge->SendId(m_sourcesMap[type]->outDir);
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
					{"outDir",m_sourcesMap["MYSQL"]->baseDir},
					{"Initd",FileExists(m_sourcesMap["MYSQL"]->baseDir + "\\my.ini")}
				}
			},
			{
				"redis",
				{
					{"InitialState",redisOut},
					{"outDir",m_sourcesMap["REDIS"]->baseDir}
				}
			}
		});
}

bool DataSourceManager::OpenWithExplorer(const std::string& path)
{
	HINSTANCE result = ShellExecuteA(
		nullptr,
		"open",
		"explorer.exe",
		path.c_str(),
		nullptr,
		SW_SHOWNORMAL
	);
	return reinterpret_cast<INT_PTR>(result) > 32;
}