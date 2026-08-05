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

//
//// 在包含 windows.h 之前减少宏污染
//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
//#endif
//#ifndef NOMINMAX
//#define NOMINMAX
//#endif
//#include <windows.h>
//
//#include <string>
//#include <vector>
//// 启动 mysql.exe 到新控制台
//// mysqlExePath: 完整的 mysql.exe 路径（UTF-16）
//// argsUtf8: 传给 mysql 的参数，例如 "-uroot -p123"（UTF-8）
//// 返回 true 表示启动成功
//inline bool OpenMysqlInNewTerminal(const std::wstring& mysqlExePath, const std::string& argsUtf8)
//{
//    // 将 UTF-8 参数转换为 UTF-16
//    int needed = MultiByteToWideChar(CP_UTF8, 0, argsUtf8.c_str(), -1, nullptr, 0);
//    if (needed == 0) {
//        return false;
//    }
//    std::vector<wchar_t> argsBuf(needed);
//    MultiByteToWideChar(CP_UTF8, 0, argsUtf8.c_str(), -1, argsBuf.data(), needed);
//
//    // 构造命令行： "C:\path\mysql.exe" <args>
//    std::wstring cmdLine;
//    cmdLine.reserve(mysqlExePath.size() + wcslen(argsBuf.data()) + 4);
//    cmdLine += L"\"";
//    cmdLine += mysqlExePath;
//    cmdLine += L"\"";
//    if (argsBuf.size() > 1) { // 有参数
//        cmdLine += L" ";
//        cmdLine += argsBuf.data();
//    }
//
//    // CreateProcessW 需要可修改的缓冲区（非 const）
//    std::vector<wchar_t> cmdLineBuf(cmdLine.begin(), cmdLine.end());
//    cmdLineBuf.push_back(0);
//
//    STARTUPINFOW si;
//    PROCESS_INFORMATION pi;
//    ZeroMemory(&si, sizeof(si));
//    si.cb = sizeof(si);
//    ZeroMemory(&pi, sizeof(pi));
//
//    // CREATE_NEW_CONSOLE 确保在新控制台窗口中运行
//    BOOL ok = CreateProcessW(
//        nullptr,                // lpApplicationName
//        cmdLineBuf.data(),      // lpCommandLine
//        nullptr,                // lpProcessAttributes
//        nullptr,                // lpThreadAttributes
//        FALSE,                  // bInheritHandles
//        CREATE_NEW_CONSOLE,     // dwCreationFlags
//        nullptr,                // lpEnvironment
//        nullptr,                // lpCurrentDirectory
//        &si,                    // lpStartupInfo
//        &pi                     // lpProcessInformation
//    );
//
//    if (!ok) {
//        // 可选：获取错误码用于调试
//        DWORD err = GetLastError();
//        (void)err;
//        return false;
//    }
//
//    // 不等待子进程，关闭句柄
//    CloseHandle(pi.hThread);
//    CloseHandle(pi.hProcess);
//    return true;
//}