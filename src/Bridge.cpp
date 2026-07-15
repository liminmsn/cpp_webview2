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
		NetContext context{
				url,
				method,
				headers ,
				[this,id](cpr::Response r) {
					nlohmann::json res;
						res["id"] = id;
						res["data"] = {
							{"status", r.status_code},
							{"body", r.text}
						};
						Send(res);
				}
		};
		Net::Net(context);
		//std::thread([this, id, url, method, headers]() {
		//	NetContext context{
		//		url,
		//		method,
		//		headers ,
		//		[this,id](cpr::Response r) {
		//			nlohmann::json res;
		//				res["id"] = id;
		//				res["data"] = {
		//					{"status", r.status_code},
		//					{"body", r.text}
		//				};
		//				Send(res.dump());
		//		}
		//	};
		//	Net::Net(context);
		//	}).detach();

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
	}
	else
	{
		// ========================
		// 默认返回
		// ========================
		Send(j);
	}
}

void Bridge::Send(const json &json)
{
	std::wstring w = Utf8ToUtf16(json.dump());

	m_app.hkwebview->webview->PostWebMessageAsJson(w.c_str());
}