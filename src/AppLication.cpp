#define UNICODE
#define _UNICODE
#include "head/Application.h"

int Application::RunMessageLoop()
{
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK Application::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Application *pThis = nullptr;

    if (msg == WM_CREATE)
    {
        CREATESTRUCTW *pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
        pThis = reinterpret_cast<Application *>(pCreate->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        pThis = reinterpret_cast<Application *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    // ========== 你要的匿名函数，替代原来独立 WndProc ==========
    auto MsgHandler = [&](HWND h, UINT m, WPARAM w, LPARAM l) -> LRESULT
    {
        switch (m)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            if (pThis && pThis->hkwebview)
                pThis->hkwebview->Resize();
            break;
        default:
            break;
        }
        return DefWindowProcW(h, m, w, l);
    };

    return MsgHandler(hwnd, msg, wParam, lParam);
}

Application::~Application()
{
    if (m_hwnd)
        DestroyWindow(m_hwnd);
    UnregisterClassW(m_className, m_hInst);
}

constexpr int DESIGN_WIDTH = 1280;
constexpr int DESIGN_HEIGHT = 720;
inline int ScaleByDpi(int value, UINT dpi)
{
    return MulDiv(value, dpi, 96);
}

Application::Application(WNDCLASSW &wc, HINSTANCE hInst, int nCmdShow) : m_wc(wc), m_hInst(hInst)
{
    m_wc.lpszClassName = m_className;
    m_wc.lpfnWndProc = Application::StaticWndProc;

    if (!RegisterClassW(&m_wc))
    {
        MessageBoxW(nullptr, L"注册窗口类失败", L"错误", MB_ICONERROR);
        return;
    }
    UINT dpi = GetDpiForSystem();
    int width = ScaleByDpi(DESIGN_WIDTH, dpi);
    int height = ScaleByDpi(DESIGN_HEIGHT, dpi);

    m_hwnd = CreateWindowExW(
        0,
        m_className,
        L"我的第一个窗口",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr, nullptr,
        m_hInst,
        this);

    if (!m_hwnd)
    {
        MessageBoxW(nullptr, L"创建窗口失败", L"错误", MB_ICONERROR);
        return;
    }

    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
    hkwebview = std::make_unique<HKWebview>(*this);
    bridge = std::make_unique<Bridge>(*this);
}