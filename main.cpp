#define UNICODE
#define _UNICODE
#include <windows.h>
#include <iostream>
#include "resource.h"
#include "src/head/AppLication.h"

static std::unique_ptr<AppLication> app;
// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"HKWebView";

    // 注册窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

    RegisterClass(&wc);

    // 创建窗口（宽字符版本）
    HWND hwnd = CreateWindowEx(
        0,                            // 扩展样式
        CLASS_NAME,                   // 类名
        L"好看韩剧5",                 // 窗口标题（宽字符）
        WS_OVERLAPPEDWINDOW,          // 样式
        CW_USEDEFAULT, CW_USEDEFAULT, // 初始位置
        800, 600,                     // 宽高
        nullptr, nullptr, hInstance, nullptr);

    if (hwnd == nullptr)
        return 0;

    AppLication::Context ctx;
    ctx.wc = wc;
    ctx.hwnd = hwnd;
    app = std::make_unique<AppLication>(ctx);

    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
