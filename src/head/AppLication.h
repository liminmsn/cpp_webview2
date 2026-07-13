#pragma once
#include <windows.h>

class AppLication
{
public:
    struct Context
    {
        WNDCLASS wc{};
        HWND hwnd{};
    };
    explicit AppLication(const AppLication::Context &context);

private:
    AppLication::Context context;
};