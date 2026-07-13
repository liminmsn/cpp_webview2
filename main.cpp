#define UNICODE
#define _UNICODE
#include "src/head/Application.h"
#include "resource.h"

static std::unique_ptr<Application> app;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASSW wc = {};
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON));
    app = std::make_unique<Application>(wc, hInstance, nCmdShow);
    int exitCode = app->RunMessageLoop();

    app.reset();
    return exitCode;
}