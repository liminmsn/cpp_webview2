#pragma once
#include <windows.h>
#include <memory>
#include "HKWebview.h"
#include "Bridge.h"
#include "Window.h"

class Application
{
public:
    explicit Application(WNDCLASSW &wc, HINSTANCE hInst, int nCmdShow);
    ~Application();
    int RunMessageLoop();
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    WNDCLASSW m_wc;
    HINSTANCE m_hInst;
    HWND m_hwnd = nullptr;
    std::unique_ptr<HKWebview> hkwebview;
    std::unique_ptr<Bridge> bridge;
    std::unique_ptr<Window> window;

private:
    const wchar_t m_className[16] = L"HKWebView";
};