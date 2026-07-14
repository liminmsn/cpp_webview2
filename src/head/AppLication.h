#pragma once
#include <windows.h>
#include <memory>
#include "HKWebview.h"

class Application
{
public:
    WNDCLASSW m_wc;
    HINSTANCE m_hInst;
    HWND m_hwnd = nullptr;
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    const wchar_t m_className[16] = L"HKWebView";
    explicit Application(WNDCLASSW &wc, HINSTANCE hInst, int nCmdShow);
    int RunMessageLoop();
    ~Application();

private:
    std::unique_ptr<HKWebview> webview;
};