#define UNICODE
#define _UNICODE
#include <objbase.h>
#include "src/head/Application.h"
#include "resource.h"

#include <shellscalingapi.h>
#pragma comment(lib, "Shcore.lib")

static std::unique_ptr<Application> app;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    {
        MessageBoxW(nullptr, L"COM 初始化失败", L"错误", MB_ICONERROR);
        return 1;
    }

    WNDCLASSW wc = {};
    wc.hbrBackground = nullptr;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
    app = std::make_unique<Application>(wc, hInstance, nCmdShow);
    int exitCode = app->RunMessageLoop();

    app.reset();
    CoUninitialize();
    return exitCode;
}