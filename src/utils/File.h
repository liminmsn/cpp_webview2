#pragma once
#include <fstream>
#include <string>
#include <filesystem>

// 写文件
inline bool WriteFile(const std::string& path, const std::string& content) {
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << content;
    ofs.close();
    return true;
}

// 判断文件是否存在
inline bool FileExists(const std::string& path) {
    return std::filesystem::exists(path);
}
