#include "cpr/cpr.h"
#include "head/Bridge.h"
#include "head/Net.h"
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
	// 1. cpr 请求
	// ========================
	if (type == "cpr")
	{
		auto query = data["query"];
		std::string url = query.value("url", "");
		std::string method = query.value("method", "");
		cpr::Header headers;
		if (query.contains("head") && query["head"].is_object()) {
			for (auto& [k, v] : query["head"].items()) {
				headers[k] = v.get<std::string>();
			}
		}
	if (url.empty() || (url.rfind("http://", 0) != 0 && url.rfind("https://", 0) != 0)) {
		nlohmann::json res;
		res["id"] = id;
		res["data"] = { {"status", -1}, {"error", "invalid url"} };
		Send(res);
	} else {
		std::thread([this, url, method, headers, id]() {
			try {
				NetContext context{
					url,
					method,
					headers,
					[this,id](cpr::Response r) {
						nlohmann::json res;
						res["id"] = id;
						// cpr 返回可能包含错误信息在 r.error.message
						if (r.status_code == 0 || !r.error.message.empty()) {
							std::string msg = r.error.message.empty() ? "request failed" : r.error.message;
							res["data"] = { {"status", 0}, {"error", msg} };
						} else {
							res["data"] = {
								{"status", r.status_code},
								{"body", r.text}
							};
						}
						Send(res);
					}
				};
				Net::Net(context);
			} catch (const std::exception& e) {
				nlohmann::json res;
				res["id"] = id;
				res["data"] = { {"status", -1}, {"error", e.what()} };
				Send(res);
			} catch (...) {
				nlohmann::json res;
				res["id"] = id;
				res["data"] = { {"status", -1}, {"error", "unknown exception"} };
				Send(res);
			}
		}).detach();
	}
	}
	// ========================
	// 窗口方法
	// ========================
	else if (type == "win")
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