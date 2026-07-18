#include "head/Taskbar.h"


Taskbar::Taskbar(HWND hwnd) :hwnd_(hwnd)
{
	HRESULT hr = CoInitialize(nullptr);

	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
		return;

	hr = CoCreateInstance(
		CLSID_TaskbarList,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&taskbar_)
	);

	if (taskbar_)
	{
		taskbar_->HrInit();
	}
}

void Taskbar::SetProgress(int current, int total)
{
	if (!taskbar_)
		return;

	if (total <= 0)
		return;

	taskbar_->SetProgressState(hwnd_, TBPF_NORMAL);
	taskbar_->SetProgressValue(hwnd_, current, total);
}



void Taskbar::Clear()
{
	if (!taskbar_)
		return;

	taskbar_->SetProgressState(hwnd_, TBPF_NOPROGRESS);
}