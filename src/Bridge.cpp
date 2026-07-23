#include "head/Bridge.h"
#include "head/Net.h"
#include "head/AppLication.h"
#include "utils/Encoding.h"

Bridge::Bridge(Application& app) : m_app(app) {}

void Bridge::Init()
{
	OutputDebugStringW(L"Bridge::Init\n");
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
	auto& obj = j["data"];
	std::string type = obj.value("type", "");

	if (type == "win")
	{
		auto action = obj["data"].value("type", "");
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
	// ========================
	// 窗口方法mysql管理
	// ========================
	else if (type == "mysql") {
		auto action = obj["data"].value("type", "");

		nlohmann::json res;
		res["id"] = id;

		auto& mysql = m_app.mysqlManager;
		if (action == "InstallService") {
			res["data"] = mysql->InstallService();
		}
		else if (action == "RemoveService")
		{
			res["data"] = mysql->RemoveService();
		}
		else if (action == "Start")
		{
			res["data"] = mysql->Start();
		}
		else if (action == "Stop")
		{
			res["data"] = mysql->Stop();
		}
		else if (action == "GetState")
		{
			auto& data = res["data"];
			data["installed"] = mysql->IsInstallService();
			data["running"] = mysql->IsRunning();
			data["status"] = mysql->IsRunning() ? "running" : "stopped";
			data["mysql_path"] = mysql->GetMysqlPath();
		}
		else if (action == "OpenDir")
		{
			ShellExecuteW(
				nullptr,
				L"open",
				mysql->GetMysqlPath().c_str(),
				nullptr,
				nullptr,
				SW_SHOWNORMAL);
		}
		else if (action == "OpenExE")
		{
			auto& info = obj["data"].value("info", "");
			if (info == "EditInI")
			{
				ShellExecuteW(
					nullptr,
					L"open",
					mysql->GetMysqlConfigPath().c_str(),
					nullptr,
					nullptr,
					SW_SHOWNORMAL);
			}
			else if (info == "Powershell")
			{
				std::wstring path = mysql->GetMysqlPath();

				std::wstring cmd =
					L"-NoExit -NoLogo -NoProfile -Command \"Set-Location -LiteralPath '" + path + L"\\bin" + L"'\"";

				ShellExecuteW(
					nullptr,
					L"open",
					L"powershell.exe",
					cmd.c_str(),
					nullptr,
					SW_SHOWNORMAL);
			}
		}
		Send(res);
	}
	else
	{
		// ========================
		// 默认返回
		// ========================
		Send(j);
	}
}