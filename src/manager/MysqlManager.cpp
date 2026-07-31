#include "../head/MysqlManager.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../head/AppLication.h"
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
						m_app.bridge->SendId({ {"type","Init"},{ "data",{{"msg",msg},{"end",false}}} });
					}))
				{
					m_app.bridge->SendId({ {"type","Init"},{ "data",{{"msg","解压完成！"},{"end",true}}} });
					//std::cout << "解压完成！" << std::endl;
				}
				else {
					m_app.bridge->SendId({ {"type","Init"},{ "data",{{"msg","解压失败！"},{"end",false}}} });
					//std::cout << "解压失败！" << std::endl;
				}
			}).detach();
	}
}
