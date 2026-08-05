#pragma once

#include <windows.h>
#include <string>

inline std::string Utf16ToUtf8(const std::wstring& wstr)
{
    if (wstr.empty())
        return {};

    int size = WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), -1,
        nullptr, 0,
        nullptr, nullptr);

    std::string result(size - 1, '\0');

    WideCharToMultiByte(
        CP_UTF8, 0,
        wstr.c_str(), -1,
        result.data(), size,
        nullptr, nullptr);

    return result;
}

inline std::wstring Utf8ToUtf16(const std::string& str)
{
    if (str.empty())
        return {};

    int size = MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        -1,
        nullptr,
        0);

    std::wstring result(size - 1, L'\0');

    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.c_str(),
        -1,
        result.data(),
        size);
    return result;
}

inline std::wstring Utf8ToWstring(const std::string& str) {
    if (str.empty()) return L"";

    int size_needed = MultiByteToWideChar(
        CP_UTF8,            // 源编码：UTF-8
        0,                  // 默认转换选项
        str.data(),         // 输入字符串
        (int)str.size(),    // 输入长度
        nullptr,            // 先不输出
        0                   // 先计算需要的长度
    );

    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8,
        0,
        str.data(),
        (int)str.size(),
        &wstr[0],
        size_needed
    );

    return wstr;
}

inline std::string WStringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return {};
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string str(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), str.data(), sizeNeeded, nullptr, nullptr);
    return str;
}