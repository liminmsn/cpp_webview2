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
	if (pos["data"] == "GetState")
		GetState();
	else if (pos["data"] == "OpenWithExplorer")
		m_app.bridge->SendId(OpenWithExplorer(pos["path"]));
	else if (pos["data" == "OpenNewTerminal"])
		m_app.bridge->SendId(false);
		//m_app.bridge->SendId(OpenMysqlInNewTerminal(pos["path"], pos["args"]));
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
	// 使用 ShellExecute 打开指定路径
	HINSTANCE result = ShellExecuteA(
		nullptr,        // 父窗口句柄
		"open",         // 动作：open
		"explorer.exe", // 程序：Explorer
		path.c_str(),   // 参数：要打开的路径
		nullptr,        // 默认目录
		SW_SHOWNORMAL   // 显示方式
	);

	// ShellExecute 返回值大于 32 表示成功
	return reinterpret_cast<int>(result) > 32;
}