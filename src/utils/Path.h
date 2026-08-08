#pragma once
#include <iostream>
#include <filesystem>
#include <appmodel.h>
#pragma comment(lib, "runtimeobject.lib")


inline std::wstring GetExeDir()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    std::wstring fullPath(buffer);

    size_t pos = fullPath.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        return fullPath.substr(0, pos);
    }
    return fullPath;
}

inline std::wstring GetInstallPath()
{
    UINT32 length = 0;
    LONG result = GetCurrentPackagePath(&length, nullptr);
    if (result == ERROR_SUCCESS && length > 0) {
        std::wstring path(length, L'\0');
        result = GetCurrentPackagePath(&length, path.data());
        if (result == ERROR_SUCCESS) {
            path.resize(length);
            return path;
        }
    }
    return GetExeDir();
}

inline bool isDirectoryEmpty(const std::string& path) {
    namespace fs = std::filesystem;
    fs::path dir(path);
    // 如果不是目录，直接返回 false
    if (!fs::is_directory(dir)) {
        return false;
    }
    // 判断目录是否为空
    return fs::directory_iterator(dir) == fs::directory_iterator();
}

// 返回目录存在且非空
inline bool directoryExistsAndNotEmpty(const std::string& path) {
    namespace fs = std::filesystem;
    fs::path dir(path);
    if (!fs::exists(dir) || !fs::is_directory(dir)) return false;
    return fs::directory_iterator(dir) != fs::directory_iterator();
}