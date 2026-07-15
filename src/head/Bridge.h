#pragma once
#include <iostream>
#include <WebView2.h>
#include "../utils/JsonUtil.h"

class Application;
class Bridge
{
public:
	explicit Bridge(Application& app);
	void Init();

private:
	void OnWebMessage(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args);
	void Send(const json& json);
	Application& m_app;
};