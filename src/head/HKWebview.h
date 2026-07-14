#pragma once
#include <windows.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <WebView2.h>

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;

class Application;
class HKWebview
{
public:
    explicit HKWebview(Application &app);
    void HKWebview::InitSettings();
    void HKWebview::InitEvents();
    void HKWebview::Resize();

private:
    ComPtr<ICoreWebView2Environment> environment;
    ComPtr<ICoreWebView2Controller> controller;
    ComPtr<ICoreWebView2> webview;
    Application &m_app;
};