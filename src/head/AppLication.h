#pragma once
#include <windows.h>
#include <memory>
#include "HKWebview.h"
#include "Bridge.h"

class Application
{
public:
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    explicit Application(WNDCLASSW &wc, HINSTANCE hInst, int nCmdShow);
    WNDCLASSW m_wc;
    HINSTANCE m_hInst;
    HWND m_hwnd = nullptr;
    std::unique_ptr<HKWebview> hkwebview;
    std::unique_ptr<Bridge> bridge;

private:
    const wchar_t m_className[16] = L"HKWebView";
    int RunMessageLoop();
    ~Application();
};