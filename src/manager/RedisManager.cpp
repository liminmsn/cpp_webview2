#include "../head/RedisManager.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../utils/File.h"
#include "../head/AppLication.h"
#include <thread>

RedisManager::RedisManager(AppLication& app) :m_app(app) {
	std::wstring wBasePath = GetInstallPath() + L"\\resources";
	std::string basePath = WStringToUtf8(wBasePath);

	fs::path zipDir = fs::path(basePath) / "zip";
	fs::path redisZip = zipDir / "Redis-8.8.1-Windows-x64-cygwin.zip";

	zipPath = redisZip.string();
	outDir = (fs::path(basePath) / "redis").string();
	baseDir += outDir + "\\Redis-8.8.1-Windows-x64-cygwin";
}

void RedisManager::Init() {
	if (!directoryExistsAndNotEmpty(outDir)) {
		std::thread(
			[&]() {
				if (ExtractZip(zipPath, outDir,
					[&](const std::string& msg) {
						m_app.bridge->SendId({ {"type","Init"},{ "data",{{"msg",msg},{"end",false}}} });
					}))
				{
					m_app.bridge->SendId({ {"type","Init"},{ "data",{{"msg","解压完成！"},{"end",true}}} });
				}
				else {
					m_app.bridge->SendId({ {"type","Init"},{ "data",{{"msg","解压失败！"},{"end",false}}} });
				}
			}).detach();
	}
}

void RedisManager::OnMessage(json& data) {
	if (data["key"] == "GetConfigFile") {
		std::string content;
		if (ReadFile(baseDir + "\\redis.conf", content)) {
			m_app.bridge->SendId(
				{
					{"code",0},
					{"data",content}
				}
			);
		}
		else {
			m_app.bridge->SendId(
				{
					{"code",1},
					{"data",nullptr}
				}
			);
		}
	}
	else if (data["key"] == "WriteConfigFile") {
		json res;
		res["state"] = false;
		if (WriteFile(baseDir + "\\redis.conf", data["content"]))
		{
			res["state"] = true;
		}
		m_app.bridge->SendId(res);
	}
	else if (data["key"] == "Run") {

	}
}