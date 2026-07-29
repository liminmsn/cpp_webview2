#include "../head/MysqlManager.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../head/AppLication.h"
#include <thread>

bool MysqlManager::Start() {
	return false;
}

bool MysqlManager::Stop() {
	return false;
}

bool MysqlManager::Init() {
	std::thread(
		[&]() {
			if (ExtractZip(zipPath, outDir,
				[&](const std::string& msg) {
					json res;
					res["data"] = msg;
					m_app.bridge->Send(res);
				}))
			{
				std::cout << "解压完成！" << std::endl;
			}
			else {
				std::cout << "解压失败！" << std::endl;
			}
		}).detach();
	return false;
}

MysqlManager::~MysqlManager() = default;
MysqlManager::MysqlManager(AppLication& app) :m_app(app)
{
	std::wstring wBasePath = GetInstallPath() + L"\\resources";
	std::string basePath = WStringToUtf8(wBasePath);

	fs::path zipDir = fs::path(basePath) / "zip";
	fs::path mysqlZip = zipDir / "mysql-8.4.10-winx64.zip";

	zipPath = mysqlZip.string();
	outDir = (fs::path(basePath) / "mysql").string();
}