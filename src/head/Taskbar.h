#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;


class Taskbar
{
public:
	explicit Taskbar(HWND hwnd);
	void SetProgress(int current, int total);
	void Clear();
private:
	HWND hwnd_{};
	ComPtr<ITaskbarList3> taskbar_;
};