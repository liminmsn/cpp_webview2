#include "head/Window.h"

#include <dwmapi.h>
#include <uxtheme.h>
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")


Window::Window(HWND hwnd) : hwnd_(hwnd) {
	DWM_SYSTEMBACKDROP_TYPE type = DWMSBT_TRANSIENTWINDOW;
	DwmSetWindowAttribute(hwnd_, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
}

void Window::Minimize()
{
	ShowWindow(hwnd_, SW_MINIMIZE);
}

void Window::Maximize()
{
	ShowWindow(hwnd_, SW_MAXIMIZE);
}

void Window::Restore()
{
	ShowWindow(hwnd_, SW_RESTORE);
}

bool Window::IsMaximized() const
{
	return IsZoomed(hwnd_);
}

bool Window::IsMinimized() const
{
	return IsIconic(hwnd_);
}

bool Window::IsFullscreen() const
{
	return fullscreen_;
}

void Window::ToggleMaximize()
{
	if (IsZoomed(hwnd_))
		ShowWindow(hwnd_, SW_RESTORE);
	else
		ShowWindow(hwnd_, SW_MAXIMIZE);
}

void Window::ToggleFullscreen()
{
	if (!fullscreen_)
	{
		// 保存当前窗口状态
		placement_.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwnd_, &placement_);

		style_ = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
		exStyle_ = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

		// 去掉窗口边框
		SetWindowLongPtr(
			hwnd_,
			GWL_STYLE,
			style_ & ~(WS_CAPTION | WS_THICKFRAME));

		SetWindowLongPtr(
			hwnd_,
			GWL_EXSTYLE,
			exStyle_ &
			~(WS_EX_DLGMODALFRAME |
				WS_EX_WINDOWEDGE |
				WS_EX_CLIENTEDGE |
				WS_EX_STATICEDGE));

		// 获取当前显示器
		HMONITOR monitor =
			MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi{};
		mi.cbSize = sizeof(mi);

		GetMonitorInfo(monitor, &mi);

		// 铺满整个屏幕
		SetWindowPos(
			hwnd_,
			HWND_TOP,
			mi.rcMonitor.left,
			mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER |
			SWP_FRAMECHANGED);

		fullscreen_ = true;
	}
	else
	{
		// 恢复窗口样式
		SetWindowLongPtr(hwnd_, GWL_STYLE, style_);
		SetWindowLongPtr(hwnd_, GWL_EXSTYLE, exStyle_);

		// 恢复窗口位置
		SetWindowPlacement(hwnd_, &placement_);

		SetWindowPos(
			hwnd_,
			nullptr,
			0,
			0,
			0,
			0,
			SWP_NOMOVE |
			SWP_NOSIZE |
			SWP_NOZORDER |
			SWP_NOOWNERZORDER |
			SWP_FRAMECHANGED);

		fullscreen_ = false;
	}
}

void EnableDarkModeWindow(HWND hwnd, BOOL useDark)
{
	// 两个常见的 DWMWA 值，某些 Windows 版本使用 20，有些使用 19
	const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_20 = 20;
	const DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_19 = 19;

	HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_20, &useDark, sizeof(useDark));
	if (FAILED(hr))
	{
		DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_19, &useDark, sizeof(useDark));
	}

	// 让常见控件使用暗色主题
	SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);
};