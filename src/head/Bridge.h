#pragma once
#include <iostream>
#include <WebView2.h>
#include "../utils/JsonUtil.h"

class Application;
class Bridge
{
public:
	explicit Bridge(Application& app);
	void Send(const json& json);
	void Init();

private:
	void OnWebMessage(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args);
	Application& m_app;
};