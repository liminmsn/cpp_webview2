#include "head/Bridge.h"
#include "head/AppLication.h"
#include "utils/Encoding.h"
#include <nlohmann/json.hpp>

Bridge::Bridge(Application &app) : m_app(app)
{
    m_app.hkwebview->webview->add_WebMessageReceived(
        Callback<ICoreWebView2WebMessageReceivedEventHandler>(
            [this](ICoreWebView2 *sender,
                   ICoreWebView2WebMessageReceivedEventArgs *args)
            {
                OnWebMessage(sender, args);
                return S_OK;
            })
            .Get(),
        nullptr);
}

void Bridge::OnWebMessage(
    ICoreWebView2 *sender,
    ICoreWebView2WebMessageReceivedEventArgs *args)
{
    LPWSTR message = nullptr;

    args->get_WebMessageAsJson(&message);

    std::wstring ws(message);
    CoTaskMemFree(message);

    std::string json = Utf16ToUtf8(ws);

    auto obj = nlohmann::json::parse(json);

    std::string event = obj["event"];

    if (event == "login")
    {
        // Login(obj["data"]);
    }
}

void Bridge::Send(const std::string &json)
{
    std::wstring w = Utf8ToUtf16(json);

    m_app.hkwebview->webview->PostWebMessageAsJson(w.c_str());
}