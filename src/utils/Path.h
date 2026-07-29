#include <Windows.h>
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