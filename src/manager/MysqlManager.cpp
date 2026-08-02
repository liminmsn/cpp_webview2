#include "../utils/Server.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../utils/File.h"
#include "../head/AppLication.h"
#include "../head/MysqlManager.h"
#include <thread>

MysqlManager::MysqlManager(AppLication& app) :m_app(app)
{
	std::wstring wBasePath = GetInstallPath() + L"\\resources";
	std::string basePath = WStringToUtf8(wBasePath);

	fs::path zipDir = fs::path(basePath) / "zip";
	fs::path mysqlZip = zipDir / "mysql-8.4.10-winx64.zip";

	zipPath = mysqlZip.string();

	outDir = (fs::path(basePath) / "mysql").string();

	if (directoryExistsAndNotEmpty(outDir)) {
		outDir += "\\mysql-8.4.10-winx64";
	}
}

bool MysqlManager::IsMySQLRunning(int port = 3306, std::string* outInfo = nullptr, int connectTimeoutMs = 500)
{
	std::ostringstream oss;
	bool ok = false;

	// Ensure Winsock is initialized here (safe even if CheckPortUsage also did it)
	WSADATA wsaData;
	bool wsaInitialized = false;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
		wsaInitialized = true;
	}
	else {
		oss << "WSAStartup failed error=" << WSAGetLastError() << "\n";
		if (outInfo) *outInfo = oss.str();
		return false;
	}

	// 1) 检查端口占用（使用你已有的函数）
	ServerCheckPortUsage usage = CheckPortUsage(port);
	if (!usage.Props.empty()) {
		oss << "Port " << port << " is used by PIDs: ";
		for (size_t i = 0; i < usage.Props.size(); ++i) {
			if (i) oss << ",";
			oss << usage.Props[i];
		}
		oss << "\n";
	}
	else {
		oss << "No process found listening on port " << port << "\n";
	}

	// 2) 尝试建立 TCP 连接到 localhost:port（非阻塞 + 超时）
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		oss << "socket() failed error=" << WSAGetLastError() << "\n";
		if (outInfo) *outInfo = oss.str();
		if (wsaInitialized) WSACleanup();
		return false;
	}

	// 设置为非阻塞
	u_long mode = 1;
	ioctlsocket(s, FIONBIO, &mode);

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)port);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

	int ret = connect(s, (sockaddr*)&addr, sizeof(addr));
	if (ret == 0) {
		ok = true;
		oss << "TCP connect succeeded immediately to 127.0.0.1:" << port << "\n";
	}
	else {
		int last = WSAGetLastError();
		if (last == WSAEWOULDBLOCK || last == WSAEINPROGRESS) {
			fd_set writeSet;
			FD_ZERO(&writeSet);
			FD_SET(s, &writeSet);
			timeval tv;
			tv.tv_sec = connectTimeoutMs / 1000;
			tv.tv_usec = (connectTimeoutMs % 1000) * 1000;
			int sel = select(0, nullptr, &writeSet, nullptr, &tv);
			if (sel > 0 && FD_ISSET(s, &writeSet)) {
				int err = 0;
				int len = sizeof(err);
				if (getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&err, &len) == 0) {
					if (err == 0) {
						ok = true;
						oss << "TCP connect succeeded to 127.0.0.1:" << port << "\n";
					}
					else {
						oss << "connect failed, socket error=" << err << "\n";
					}
				}
				else {
					oss << "getsockopt failed error=" << WSAGetLastError() << "\n";
				}
			}
			else if (sel == 0) {
				oss << "connect timeout after " << connectTimeoutMs << "ms\n";
			}
			else {
				oss << "select failed error=" << WSAGetLastError() << "\n";
			}
		}
		else {
			oss << "connect immediate error=" << last << "\n";
		}
	}

	// 恢复阻塞模式（可选）
	mode = 0;
	ioctlsocket(s, FIONBIO, &mode);
	closesocket(s);

	// 可选：如果 TCP 成功，进一步用 mysqladmin ping 验证（注：需要正确的 mysqladmin 路径/权限）
	if (ok) {
		// 下面示例使用你已有的 RunCommandWithOutput 来执行 mysqladmin ping（非阻塞）
		// 注意：如果 mysqladmin 需要密码，你需要传入 -pYourPassword 或者使用无密码环境
		std::string pingCmd = ".\\mysqladmin.exe ping";
		std::string pingOut;
		RunCommandWithOutput(pingCmd, [&](const std::string& s) { pingOut += s; });
		oss << "mysqladmin ping output:\n" << pingOut << "\n";
		// 如果输出包含 "mysqld is alive" 则确认服务协议可用
		if (pingOut.find("mysqld is alive") != std::string::npos) {
			oss << "mysqladmin ping confirms server is alive.\n";
			if (outInfo) *outInfo = oss.str();
			if (wsaInitialized) WSACleanup();
			return true;
		}
		else {
			oss << "mysqladmin ping did not confirm server alive (output above).\n";
			// 仍然把 TCP 结果作为可用依据，或根据需要返回 false
			// 这里我们选择返回 true because TCP connect succeeded; change if you prefer stricter check
			if (outInfo) *outInfo = oss.str();
			if (wsaInitialized) WSACleanup();
			return true;
		}
	}

	if (outInfo) *outInfo = oss.str();
	if (wsaInitialized) WSACleanup();
	return ok;
}

void MysqlManager::Init() {
	if (!directoryExistsAndNotEmpty(outDir)) {
		std::thread(
			[&]() {
				if (ExtractZip(zipPath, outDir,
					[&](const std::string& msg) {
						m_app.bridge->SendId({
							{"type","Init"},
							{ "data",{
								{"msg",msg},
								{"end",false}
							}}
							});
					}))
				{
					m_app.bridge->SendId({
						{"type","Init"},
						{ "data",{
							{"msg","解压完成！"},
							{"end",true}
						}}
						});
				}
				else {
					m_app.bridge->SendId({
						{"type","Init"},
						{ "data",{
							{"msg","解压失败！"},
							{"end",false}
						}}
						});
				}
			}).detach();
	}
}

void MysqlManager::Send(std::string msg) {
	json res;
	res["type"] = "MysqlLog";
	res["msg"] = msg;
	m_app.bridge->SendId(res);
}

bool MysqlManager::StopMysqldUsingMysqladmin(const std::string& binDir, const std::string& mysqladminArgs = "-u root -p", int port = 3306, int waitMs = 5000)
{
	std::string exe = binDir;
	if (!exe.empty() && (exe.back() != '\\' && exe.back() != '/')) exe += "\\";
	exe += "mysqladmin.exe";
	std::string cmd = "\"" + exe + "\" " + mysqladminArgs + " shutdown";

	std::string output;
	RunCommandWithOutput(cmd,
		[&](const std::string& s) {
			Send(output += s);
		});
	const int stepMs = 200;
	int waited = 0;
	while (waited < waitMs) {
		ServerCheckPortUsage usage = CheckPortUsage(port);
		if (usage.Props.empty()) {
			return true;
		}
		Sleep(stepMs);
		waited += stepMs;
	}
	return false;
}

void MysqlManager::OnMessage(json& data) {
	std::string workDir = outDir + "\\bin";
	std::string mysqld = outDir + "\\bin\\mysqld.exe";

	if (data["key"] == "CreateConfig")
	{
		std::string& ConfigLabel = data["val"].get<std::string>();
		if (WriteFile(outDir + "\\my.ini", ConfigLabel) && CreateDirectory(outDir, "data")) {
			if (directoryExistsAndNotEmpty(outDir + "\\data")) {
				Send("已经初始化成功过了！");
				return;
			}

			std::string cmd = "\"" + mysqld + "\" --initialize-insecure --console";
			RunCommandWithOutput(cmd, [&](const std::string& output) {
				Send(output);
				});
			Send("写入配置成功！");
		}
		else
		{
			Send("写入配置失败！");
		}
	}
	else if (data["key"] == "Initd")
	{
		bool initd = directoryExistsAndNotEmpty(outDir + "\\data") && FileExists(outDir + "\\my.ini");
		m_app.bridge->SendId(initd);
	}
	else if (data["key"] == "Run") {
		std::string cmd = "\"" + mysqld + "\" --defaults-file=\"..\\my.ini\" --console";

		if (StartProcessAndStreamOutput(cmd,
			[&](const std::string& output) {
				Send(output);
			}, nullptr, workDir))
		{
			m_app.bridge->SendId(true);
		}
		else {
			m_app.bridge->SendId(false);
		}
	}
	else if (data["key"] == "Stop") {
		std::string mysqladminArgs = data["mysqladminArgs"].get<std::string>();
		StopMysqldUsingMysqladmin(workDir, mysqladminArgs);
	}
	else if (data["key"] == "IsRun") {
		m_app.bridge->SendId(IsMySQLRunning());
	}
}