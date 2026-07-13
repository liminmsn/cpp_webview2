#include "head/AppLication.h"

AppLication::AppLication(const AppLication::Context &context) : context(context)
{
    ShowWindow(context.hwnd, SW_SHOW);
    UpdateWindow(context.hwnd);
}