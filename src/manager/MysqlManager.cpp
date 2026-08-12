#include "../utils/Server.h"
#include "../utils/Encoding.h"
#include "../utils/Path.h"
#include "../utils/Zip.h"
#include "../utils/File.h"
#include "../head/AppLication.h"
#include "../head/MysqlManager.h"
#include <tlhelp32.h>
#include <utils.h>
#include <thread>



//默认初始化
MysqlManager::MysqlManager(AppLication& app) :m_app(app)
{
	std::wstring wBasePath = GetInstallPath() + L"\\resources";
	std::string basePath = WStringToUtf8(wBasePath);

	fs::path zipDir = fs::path(basePath) / "zip";
	zipPath = (zipDir / "mysql-8.4.10-winx64.zip").string();

#ifdef WEBVIEW_DEBUG
	outDir = (fs::path(basePath) / "mysql").string();
#else
	//std::wstring localAppData = GetLocalAppDataPath();
	//outDir = WStringToUtf8(localAppData + L"\\LocalRM\\mysql");
	std::wstring localState = GetSandboxLocalStatePath();
	outDir = WStringToUtf8(localState + L"\\LocalRM\\mysql");
#endif
	baseDir = outDir + "\\mysql-8.4.10-winx64";

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
			fd_set writeSet = {};
			FD_ZERO(&writeSet);
			FD_SET(s, &writeSet);
			timeval tv = {};
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

bool MysqlManager::InitializeMysql()
{
	if (IsMysqldProcessRunning())
	{
		Send("检测到 mysqld.exe 正在运行，禁止初始化！");
		return false;
	}

	std::string binDir = baseDir + "\\bin";
	std::string mysqld =
		binDir + "\\mysqld.exe";

	SetDllDirectoryA(binDir.c_str());
	std::string cmd =
		"\"" + mysqld +
		"\" --defaults-file=\"" +
		baseDir +
		"\\my.ini\" --initialize --console";
	bool success = true;

	RunCommandWithOutput(
		cmd,
		[&](const std::string& output)
		{
			Send(output);

			if (output.find("error") != std::string::npos ||
				output.find("failed") != std::string::npos)
			{
				success = false;
			}
		},
		baseDir + "\\bin"
	);


	return success;
}

bool MysqlManager::IsMysqldProcessRunning()
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0
	);

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W);

	bool found = false;

	if (Process32FirstW(hSnapshot, &pe))
	{
		do
		{
			if (_wcsicmp(pe.szExeFile, L"mysqld.exe") == 0)
			{
				found = true;
				break;
			}

		} while (Process32NextW(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);

	return found;
}

void MysqlManager::OnMessage(json& data) {
	std::string workDir = baseDir + "\\bin";
	std::string mysqld = baseDir + "\\bin\\mysqld.exe";
	std::string mysqladmin = baseDir + "\\bin\\mysqladmin.exe";


	if (data["key"] == "Initd")
		m_app.bridge->SendId(FileExists(baseDir + "\\my.ini"));
	if (data["key"] == "InitdServer")
		m_app.bridge->SendId(directoryExistsAndNotEmpty(baseDir + "\\data"));
	else if (data["key"] == "InitializeMysql")
		m_app.bridge->SendId(InitializeMysql());
	else if (data["key"] == "IsRun")
		m_app.bridge->SendId(IsMySQLRunning());
	else if (data["key"] == "CreateConfig")
	{
		std::string ConfigLabel = data["val"].get<std::string>();
		std::string dataDir = baseDir + "\\data";
		if (WriteFile(baseDir + "\\my.ini", ConfigLabel) && RemoveDirectory(dataDir))
		{
			// 创建 data 目录
			if (!CreateDirectoryA(dataDir.c_str(), nullptr))
			{
				DWORD err = GetLastError();
				if (err != ERROR_ALREADY_EXISTS)
				{
					Send("创建data目录失败！");
					return;
				}
			}

			if (directoryExistsAndNotEmpty(dataDir)) {
				Send("重写配置成功！");
				return;
			}
			Send("写入配置成功！");
		}
		else Send("写入配置失败！");
	}
	else if (data["key"] == "Run") {
		std::string cmd = "\"" + mysqld + "\" --defaults-file=\"..\\my.ini\" --console";
		if (StartProcessAndStreamOutput(cmd,
			[&](const std::string& output) {
				Send(output);
			}, nullptr, workDir))
			m_app.bridge->SendId(true);
		else
			m_app.bridge->SendId(false);
	}
	else if (data["key"] == "Stop") {
		std::string Args = data["mysqladminArgs"].get<std::string>();
		std::string args = Args + " shutdown";

		std::wstring mysqladminPath = Utf8ToUtf16(mysqladmin);
		bool res = utils.execute_process(
			mysqladminPath.c_str(),
			args.c_str(),
			false
		);
		m_app.bridge->SendId(res);
	}
}