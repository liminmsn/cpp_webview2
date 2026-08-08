#pragma once
#include <fstream>
#include <string>
#include <filesystem>

inline bool WriteFile(const std::string& path, const std::string& content)
{
	std::ofstream ofs(path, std::ios::trunc);

	if (!ofs)
	{
		return false;
	}

	ofs << content;

	return true;
}

// 判断文件是否存在
inline bool FileExists(const std::string& path) {
	return std::filesystem::exists(path);
}

// 在指定路径下创建指定名字的目录
inline bool CreateDirectory(const std::string& basePath, const std::string& dirName) {
	std::filesystem::path fullPath = std::filesystem::path(basePath) / dirName;
	try {
		if (!std::filesystem::exists(fullPath)) {
			return std::filesystem::create_directories(fullPath);
		}
		return true; // 已存在就算成功
	}
	catch (const std::filesystem::filesystem_error& e) {
		return false;
	}
}

// 删除指定目录
inline bool RemoveDirectory(const std::string& path) {
	try {
		if (!std::filesystem::exists(path)) {
			return true;
		}

		std::filesystem::remove_all(path);
		return true;
	}
	catch (const std::filesystem::filesystem_error&) {
		return false;
	}
}

// 删除目录下所有内容（保留目录）
inline bool ClearDirectory(const std::string& path) {
	try {
		if (!std::filesystem::exists(path)) {
			return true;
		}

		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			std::filesystem::remove_all(entry.path());
		}

		return true;
	}
	catch (const std::filesystem::filesystem_error&) {
		return false;
	}
}

//读取文件内容
inline bool ReadFile(const std::string& path, std::string& content) {
	std::ifstream ifs(path, std::ios::in | std::ios::binary);
	if (!ifs.is_open()) {
		return false;
	}

	content.assign(
		std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>()
	);
	ifs.close();
	return true;
}