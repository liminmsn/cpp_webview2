#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <WebView2.h>

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;


class AppLication;


class HKWebview
{
public:

    explicit HKWebview(AppLication& app);


    void Resize();


    ComPtr<ICoreWebView2Environment> environment;
    ComPtr<ICoreWebView2Controller> controller;
    ComPtr<ICoreWebView2> webview;


private:

    void InitSettings();

    void InitEvents();


private:

    AppLication& m_app;
};