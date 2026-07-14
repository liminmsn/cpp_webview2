#include "head/HKWebview.h"
#include "head/Application.h"

HKWebview::HKWebview(Application &app) : m_app(app)
{

    CreateCoreWebView2EnvironmentWithOptions(
        nullptr,
        nullptr,
        nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment *env) -> HRESULT
            {
                if (FAILED(result))
                {
                    OutputDebugStringW(L"CreateCoreWebView2Environment failed\n");
                    return result;
                }

                environment = env;

                return environment->CreateCoreWebView2Controller(
                    m_app.m_hwnd,
                    Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                        [this](HRESULT result, ICoreWebView2Controller *controllerPtr) -> HRESULT
                        {
                            if (FAILED(result))
                            {
                                OutputDebugStringW(L"CreateCoreWebView2Controller failed\n");
                                return result;
                            }
                            controller = controllerPtr;
                            if (!controller)
                                return E_POINTER;

                            /*
                                设置WebView2透明背景
                            */
                            ComPtr<ICoreWebView2Controller2> controller2;
                            if (SUCCEEDED(controller.As(&controller2)))
                            {
                                COREWEBVIEW2_COLOR color{0, 0, 0, 0};
                                controller2->put_DefaultBackgroundColor(color);
                            }

                            controller->get_CoreWebView2(&webview);

                            if (!webview)
                                return E_POINTER;

                            RECT rc{};
                            GetClientRect(m_app.m_hwnd, &rc);
                            controller->put_Bounds(rc);
                            controller->put_IsVisible(TRUE);
                            controller->put_ZoomFactor(1.0);

                            InitSettings();
                            InitEvents();

                            webview->Navigate(L" http://127.0.0.1:5173");

                            if (m_app.bridge)
                            {
                                m_app.bridge->Init();
                            }

                            return S_OK;
                        })
                        .Get());
            })
            .Get());
}

void HKWebview::InitSettings()
{

    if (!webview)
        return;

    ComPtr<ICoreWebView2Settings> settings;
    webview->get_Settings(&settings);

    if (!settings)
        return;
    settings->put_IsScriptEnabled(TRUE);
    settings->put_IsWebMessageEnabled(TRUE);
    settings->put_AreDevToolsEnabled(TRUE);
    settings->put_AreDefaultContextMenusEnabled(FALSE);
    settings->put_IsZoomControlEnabled(FALSE);
}

void HKWebview::InitEvents()
{

    if (!webview)
        return;

    webview->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [](ICoreWebView2 *, ICoreWebView2NavigationCompletedEventArgs *) -> HRESULT
            {
                OutputDebugStringW(
                    L"WebView2 页面加载完成\n");

                return S_OK;
            })
            .Get(),
        nullptr);
}

void HKWebview::Resize()
{

    if (!controller)
        return;
    RECT rc{};
    GetClientRect(m_app.m_hwnd, &rc);
    controller->put_Bounds(rc);
}