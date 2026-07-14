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
                    OutputDebugStringW(L"CreateCoreWebView2EnvironmentWithOptions failed\n");
                    return result;
                }

                environment = env;

                return env->CreateCoreWebView2Controller(
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

                            controller->get_CoreWebView2(&webview);

                            RECT rc{};
                            GetClientRect(m_app.m_hwnd, &rc);
                            controller->put_Bounds(rc);
                            controller->put_IsVisible(TRUE);
                            controller->put_ZoomFactor(1.0);

                            InitSettings();
                            InitEvents();

                            if (webview)
                                webview->Navigate(L"https://www.baidu.com");

                            m_app.bridge->Init();
                            
                            return S_OK;
                        })
                        .Get());
            })
            .Get());
}

void HKWebview::InitSettings()
{
    ComPtr<ICoreWebView2Settings> settings;

    webview->get_Settings(&settings);

    settings->put_IsScriptEnabled(TRUE);

    settings->put_IsWebMessageEnabled(TRUE);

    settings->put_AreDevToolsEnabled(TRUE);

    settings->put_AreDefaultContextMenusEnabled(FALSE);

    settings->put_IsZoomControlEnabled(FALSE);
}

void HKWebview::InitEvents()
{
    webview->add_NavigationCompleted(
        Callback<ICoreWebView2NavigationCompletedEventHandler>(
            [](ICoreWebView2 *,
               ICoreWebView2NavigationCompletedEventArgs *)
                -> HRESULT
            {
                OutputDebugStringW(L"页面加载完成\n");
                return S_OK;
            })
            .Get(),
        nullptr);
}

void HKWebview::Resize()
{
    if (!controller)
        return;

    RECT rc;
    GetClientRect(m_app.m_hwnd, &rc);

    controller->put_Bounds(rc);
}