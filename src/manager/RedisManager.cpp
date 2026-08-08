#include "../head/RedisManager.h"
#include "../utils/Server.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../utils/File.h"
#include "../head/AppLication.h"
#include <tlhelp32.h>
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

void RedisManager::Send(std::string msg) {
	json res;
	res["type"] = "RedisLog";
	res["msg"] = msg;
	m_app.bridge->SendId(res);
}

bool RedisManager::IsRedisProcessRunning()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return false;
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W);
	bool found = false;
	if (Process32FirstW(hSnapshot, &pe))
	{
		do
		{
			if (_wcsicmp(pe.szExeFile, L"redis-server.exe") == 0)
			{
				found = true;
				break;
			}

		} while (Process32NextW(hSnapshot, &pe));
	}
	CloseHandle(hSnapshot);
	return found;
}

static PROCESS_INFORMATION redisPi{};

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
			res["state"] = true;baseDir;
		}
		else
			m_app.bridge->SendId(res);
	}
	else if (data["key"] == "IsRun") {
		m_app.bridge->SendId(IsRedisProcessRunning());
	}
	else if (data["key"] == "RUN") {
		std::string cmd =
			"\"" +
			baseDir +
			"\\redis-server.exe\" redis.conf";

		bool ok = StartProcessAndStreamOutput(
			cmd,
			[&](const std::string& output) {
				Send(output);
			},
			&redisPi,
			baseDir
		);
		if (ok)
		{
			m_app.bridge->SendId(true);
		}
		else
			m_app.bridge->SendId(false);
	}
	else if (data["key"] == "STOP") {
		if (redisPi.hProcess)
		{
			StopChildProcess(
				&redisPi
			);

			CloseHandle(redisPi.hProcess);
			CloseHandle(redisPi.hThread);

			ZeroMemory(
				&redisPi,
				sizeof(redisPi)
			);
		}
		else
		{
			KillPort(6379);
		}

		m_app.bridge->SendId(false);
	}
}