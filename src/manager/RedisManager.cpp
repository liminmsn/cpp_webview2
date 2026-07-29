#include "../head/RedisManager.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"

bool RedisManager::Start() {
	return false;
}

bool RedisManager::Stop() {
	return false;
}

bool RedisManager::Init() {

	return false;
}

RedisManager::RedisManager() {
	std::wstring path = GetInstallPath();
	std::string basePath = WStringToUtf8(GetInstallPath());
	fs::path zipDir = fs::path(basePath) / "zip";
	fs::path redisZip = zipDir / "Redis-8.8.1-Windows-x64-cygwin.zip";

	zipPath = redisZip.string();
	outDir = (fs::path(basePath) / "redis").string();
}
RedisManager::~RedisManager() {}