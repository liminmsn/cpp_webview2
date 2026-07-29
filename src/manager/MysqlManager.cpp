#include "../head/MysqlManager.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"

bool MysqlManager::Start() {
	return false;
}

bool MysqlManager::Stop() {
	return false;
}

bool MysqlManager::Init() {
	if (ExtractZip(zipPath, outDir)) {
		std::cout << "解压完成！" << std::endl;
	}
	else {
		std::cout << "解压失败！" << std::endl;
	}
	return false;
}

MysqlManager::~MysqlManager() = default;
MysqlManager::MysqlManager() {
	std::string basePath = WStringToUtf8(GetInstallPath());
	fs::path zipDir = fs::path(basePath) / "zip";
	fs::path mysqlZip = zipDir / "mysql-8.4.10-winx64.zip";

	zipPath = mysqlZip.string();
	outDir = (fs::path(basePath) / "mysql").string();
}