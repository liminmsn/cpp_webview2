#include "../utils/Server.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../utils/File.h"
#include "../head/AppLication.h"
#include "../head/MysqlManager.h"
#include <thread>

MysqlManager::MysqlManager(AppLication& app) :m_app(app)
{
	std::wstring wBasePath = GetInstallPath() + L"\\resources";
	std::string basePath = WStringToUtf8(wBasePath);

	fs::path zipDir = fs::path(basePath) / "zip";
	fs::path mysqlZip = zipDir / "mysql-8.4.10-winx64.zip";

	zipPath = mysqlZip.string();

	outDir = (fs::path(basePath) / "mysql").string();

	if (directoryExistsAndNotEmpty(outDir)) {
		outDir += "\\mysql-8.4.10-winx64";
	}
}

void MysqlManager::Init() {
	if (!directoryExistsAndNotEmpty(outDir)) {
		std::thread(
			[&]() {
				if (ExtractZip(zipPath, outDir,
					[&](const std::string& msg) {
						m_app.bridge->SendId({
							{"type","Init"},
							{ "data",{
								{"msg",msg},
								{"end",false}
							}}
							});
					}))
				{
					m_app.bridge->SendId({
						{"type","Init"},
						{ "data",{
							{"msg","解压完成！"},
							{"end",true}
						}}
						});
				}
				else {
					m_app.bridge->SendId({
						{"type","Init"},
						{ "data",{
							{"msg","解压失败！"},
							{"end",false}
						}}
						});
				}
			}).detach();
	}
}

void MysqlManager::Send(std::string msg) {
	json res;
	res["type"] = "MysqlLog";
	res["msg"] = msg;
	m_app.bridge->SendId(res);
}

void MysqlManager::OnMessage(json& data) {
	std::string workDir = outDir + "\\bin";
	std::string mysqld = outDir + "\\bin\\mysqld.exe";

	if (data["key"] == "CreateConfig")
	{
		std::string& ConfigLabel = data["val"].get<std::string>();
		if (WriteFile(outDir + "\\my.ini", ConfigLabel) && CreateDirectory(outDir, "data")) {
			if (directoryExistsAndNotEmpty(outDir + "\\data")) {
				Send("已经初始化成功过了！");
				return;
			}

			std::string cmd = "\"" + mysqld + "\" --initialize-insecure --console";
			RunCommandWithOutput(cmd, [&](const std::string& output) {
				Send(output);
				});
			Send("写入配置成功！");
		}
		else
		{
			Send("写入配置失败！");
		}
	}
	else if (data["key"] == "Initd")
	{
		bool initd = directoryExistsAndNotEmpty(outDir + "\\data") && FileExists(outDir + "\\my.ini");
		m_app.bridge->SendId(initd);
	}
	else if (data["key"] == "Run") {
		std::string cmd = "\"" + mysqld + "\" --defaults-file=\"..\\my.ini\" --console";

		if (StartProcessAndStreamOutput(cmd,
			[&](const std::string& output) {
				Send(output);
			}, outPi, workDir))
		{
			m_app.bridge->SendId(true);
		}
		else {
			m_app.bridge->SendId(false);
		}
	}
	else if (data["key"] == "Stop") {
		m_app.bridge->SendId(StopChildProcess(outPi));
	}
}