#pragma once
#include <windows.h>

class Window
{
public:
    explicit Window(HWND hwnd);

    void Minimize();
    void Maximize();
    void Restore();

    void ToggleMaximize();
    void ToggleFullscreen();

    bool IsMaximized() const;
    bool IsMinimized() const;
    bool IsFullscreen() const;

private:
    HWND hwnd_;

    bool fullscreen_ = false;

    WINDOWPLACEMENT placement_{ sizeof(WINDOWPLACEMENT) };

    DWORD style_{};
    DWORD exStyle_{};
};


void EnableDarkModeWindow(HWND hwnd,BOOL useDark);