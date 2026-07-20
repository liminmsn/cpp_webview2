#include "head/Bridge.h"
#include "head/AppLication.h"
#include "utils/Encoding.h"

Bridge::Bridge(Application& app) : m_app(app) {}

void Bridge::Init()
{
	m_app.hkwebview->webview->add_WebMessageReceived(
		Callback<ICoreWebView2WebMessageReceivedEventHandler>(
			[this](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args)
			{
				OnWebMessage(sender, args);
				return S_OK;
			})
		.Get(),
		nullptr);
}

void Bridge::OnWebMessage(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args)
{
	LPWSTR message = nullptr;
	args->get_WebMessageAsJson(&message);

	std::wstring ws(message);
	CoTaskMemFree(message);

	std::string json = Utf16ToUtf8(ws);
	auto j = StringToJson(json);
	//printf("Received message: %s\n", json.c_str());

	std::string id = j.value("id", "");
	auto data = j["data"];
	std::string type = data.value("type", "");
	// ========================
	// 窗口方法
	// ========================
	if (type == "win")
	{
		auto action = data["data"].value("type", "");
		if (action == "toggleFullscreen")
		{
			m_app.window->ToggleFullscreen();
			nlohmann::json res;
			res["id"] = id;
			res["data"] = {
				{"state", m_app.window->IsFullscreen()}
			};
			Send(res);
		}
		else if (action == "Reset") {
			if (m_app.window->IsFullscreen()) {
				m_app.window->ToggleFullscreen();
			}
		}
	}
	else
	{
		// ========================
		// 默认返回
		// ========================
		Send(j);
	}
}

void Bridge::Send(const json& json)
{
	// Post the JSON to the UI thread to ensure WebView2 COM calls happen on the
	// UI thread. Allocate the string on the heap and send via PostMessage; the
	// Application message handler will forward it to WebView2 and free it.
	std::wstring* w = new std::wstring(Utf8ToUtf16(json.dump()));
	if (m_app.m_hwnd)
	{
		PostMessageW(m_app.m_hwnd, WM_APP + 1, 0, reinterpret_cast<LPARAM>(w));
	}
	else
	{
		// Fallback: if no window, attempt direct call (best-effort) and free
		if (m_app.hkwebview)
		{
			m_app.hkwebview->webview->PostWebMessageAsJson(w->c_str());
		}
		delete w;
	}
}