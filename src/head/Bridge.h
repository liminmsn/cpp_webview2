#pragma once
#include <iostream>
#include <WebView2.h>
#include "../utils/JsonUtil.h"

class AppLication;
class Bridge
{
public:
	explicit Bridge(AppLication& app);
	void SendId(const json& data);
	void Send(const json& json);
	void Init();

private:
	void OnWebMessage(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args);
	AppLication& m_app;
	std::string id;
};