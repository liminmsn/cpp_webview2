#pragma once
#include <iostream>

class Application;
class Bridge
{
public:
    explicit Bridge(Application &app);

private:
    void OnWebMessage(
        ICoreWebView2 *sender,
        ICoreWebView2WebMessageReceivedEventArgs *args);
    void Send(const std::string &json);
    Application &m_app;
};