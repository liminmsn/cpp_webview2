#pragma once
#include <windows.h>
#include <memory>

class Application
{
public:
    explicit Application(WNDCLASSW& wc, HINSTANCE hInst, int nCmdShow);
    ~Application();

    // 单独启动消息循环，移出构造函数
    int RunMessageLoop();

    // 静态外壳，内部使用 lambda 处理消息
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    WNDCLASSW m_wc;
    HWND m_hwnd = nullptr;
    HINSTANCE m_hInst;
    int m_nCmdShow;
    const wchar_t m_className[16] = L"HKWebView";
};