#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <miniz.h>
#include <fstream>
#include "head/MysqlManager.h"
#include "head/AppLication.h"

#pragma comment(lib, "Advapi32.lib")
namespace fs = std::filesystem;
constexpr wchar_t SERVICE_NAME[] = L"LocalMysql";

MysqlManager::MysqlManager(const Application& app) :m_app(app) {};
MysqlManager::~MysqlManager() = default;

bool MysqlManager::Init() {
	wchar_t exePath[MAX_PATH]{};
	GetModuleFileNameW(nullptr, exePath, MAX_PATH);
	exePath_ = std::filesystem::path(exePath).parent_path();
	resourcePath_ = exePath_ / L"resources";

	wchar_t localAppData[MAX_PATH]{};
	if (FAILED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, localAppData)))
	{
		return false;
	}

	rootPath_ = std::filesystem::path(localAppData) / SERVICE_NAME;
	mysqlPath_ = rootPath_ / L"mysql";
	dataPath_ = mysqlPath_ / L"data";
	configPath_ = mysqlPath_ / L"my.ini";

	// 创建工作目录
	std::filesystem::create_directories(rootPath_);

	return true;
}

bool MysqlManager::Install() {
	if (installed_)
		return true;

	if (!ExtractZip())
		return false;

	if (!CreateConfig())
		return false;

	if (!Initialize())
		return false;

	//if (!InstallService())
	//	return false;

	installed_ = true;
	return true;
}


bool MysqlManager::IsInstalled() const {
	return installed_;
}

bool MysqlManager::IsInitialized() const {
	return fs::exists(dataPath_ / L"mysql.ibd") && fs::exists(dataPath_ / L"ibdata1");
}

bool MysqlManager::CreateConfig()
{
	namespace fs = std::filesystem;

	try
	{
		fs::create_directories(rootPath_);
		fs::create_directories(dataPath_);

		std::ofstream out(configPath_);

		if (!out.is_open())
			return false;

		out << "[mysqld]\n";
		out << "console\n";
		out << "basedir=" << mysqlPath_.generic_string() << "\n";
		out << "datadir=" << dataPath_.generic_string() << "\n";
		out << "port=3306\n";
		out << "character-set-server=utf8mb4\n";
		out << "collation-server=utf8mb4_general_ci\n";
		out << "default-time-zone=+08:00\n";
		out << "max_connections=200\n";
		out << "max_allowed_packet=64M\n";
		out << "sql_mode=STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION\n";
		out << "\n";
		out << "[client]\n";
		out << "port=3306\n";
		out << "default-character-set=utf8mb4\n";
		out.close();

		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool MysqlManager::ExtractZip()
{
	// 判断是否已安装
	if (std::filesystem::exists(mysqlPath_ / L"bin" / L"mysqld.exe"))
		return true;

	namespace fs = std::filesystem;
	fs::path zipPath = resourcePath_ / L"mysql-8.4.10-winx64.zip";

	if (!fs::exists(zipPath))
		return false;

	fs::path tempPath = rootPath_ / L"temp";

	if (fs::exists(tempPath))
		fs::remove_all(tempPath);

	fs::create_directories(tempPath);


	mz_zip_archive zip{};

	std::string zipFile = zipPath.string();

	if (!mz_zip_reader_init_file(
		&zip,
		zipFile.c_str(),
		0))
	{
		return false;
	}


	mz_uint fileCount = mz_zip_reader_get_num_files(&zip);

	for (mz_uint i = 0; i < fileCount; i++)
	{
		mz_zip_archive_file_stat stat{};

		if (!mz_zip_reader_file_stat(
			&zip,
			i,
			&stat))
		{
			mz_zip_reader_end(&zip);
			return false;
		}

		if (m_app.taskbar)
		{
			m_app.taskbar->SetProgress(
				i + 1,
				fileCount
			);
		}
		// 当前解压文件
		std::wstring fileName(
			stat.m_filename,
			stat.m_filename + strlen(stat.m_filename)
		);
		wprintf(L"正在解压: %s\n", fileName.c_str());




		fs::path output = tempPath / stat.m_filename;

		// 目录
		if (mz_zip_reader_is_file_a_directory(&zip, i))
		{
			fs::create_directories(output);
			continue;
		}

		fs::create_directories(output.parent_path());

		if (!mz_zip_reader_extract_to_file(
			&zip,
			i,
			output.string().c_str(),
			0))
		{
			mz_zip_reader_end(&zip);
			return false;
		}
	}

	m_app.taskbar->Clear();

	mz_zip_reader_end(&zip);

	// mysql-8.4.10-winx64目录
	fs::path extracted = tempPath / L"mysql-8.4.10-winx64";

	if (!fs::exists(extracted))
		return false;

	if (fs::exists(mysqlPath_))
		fs::remove_all(mysqlPath_);
	fs::copy(extracted, mysqlPath_, fs::copy_options::recursive);
	fs::remove_all(tempPath);

	return true;
}

bool MysqlManager::Initialize()
{
	namespace fs = std::filesystem;

	fs::path mysqld = mysqlPath_ / L"bin" / L"mysqld.exe";

	if (!fs::exists(mysqld))
		return false;

	if (!fs::exists(configPath_))
	{
		if (!CreateConfig())
			return false;
	}

	// 已初始化
	if (fs::exists(dataPath_ / L"ibdata1") &&
		fs::exists(dataPath_ / L"mysql"))
	{
		return true;
	}

	// 清理残留的初始化目录
	if (fs::exists(dataPath_))
	{
		std::error_code ec;
		fs::remove_all(dataPath_, ec);
	}

	std::error_code ec;
	fs::create_directories(dataPath_, ec);

	std::wstring command =
		L"\"" + mysqld.wstring() + L"\" "
		L"--defaults-file=\"" + configPath_.wstring() + L"\" "
		L"--initialize-insecure";

	STARTUPINFOW si{};
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi{};

	std::wstring workDir = mysqlPath_.wstring();
	std::vector<wchar_t> cmd(command.begin(), command.end());
	cmd.push_back(L'\0');

	if (!CreateProcessW(
		nullptr,
		cmd.data(),
		nullptr,
		nullptr,
		FALSE,
		CREATE_NO_WINDOW,
		nullptr,
		workDir.c_str(),
		&si,
		&pi))
	{
		return false;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	DWORD exitCode = 1;
	GetExitCodeProcess(pi.hProcess, &exitCode);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	if (exitCode != 0)
		return false;

	// 等待初始化文件真正落盘
	for (int i = 0; i < 100; ++i)
	{
		if (fs::exists(dataPath_ / L"ibdata1") &&
			fs::exists(dataPath_ / L"mysql"))
		{
			return true;
		}

		Sleep(100);
	}

	return false;
}

bool MysqlManager::IsInstallService()
{
	SC_HANDLE scm = OpenSCManagerW(
		nullptr,
		nullptr,
		SC_MANAGER_CONNECT);

	if (!scm)
		return false;

	SC_HANDLE service = OpenServiceW(
		scm,
		SERVICE_NAME,
		SERVICE_QUERY_STATUS);

	if (service)
	{
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return true;
	}

	CloseServiceHandle(scm);
	return false;
}
bool MysqlManager::InstallService()
{
	std::filesystem::path mysqld = mysqlPath_ / L"bin" / L"mysqld.exe";

	if (!std::filesystem::exists(mysqld))
		return false;


	SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);

	if (!scm)
		return false;


	// 检查服务是否存在
	SC_HANDLE oldService = OpenServiceW(scm, SERVICE_NAME, SERVICE_QUERY_STATUS);

	if (oldService)
	{
		CloseServiceHandle(oldService);
		CloseServiceHandle(scm);
		return true;
	}

	// mysqld启动命令
	std::wstring command =
		L"\"" +
		mysqld.wstring() +
		L"\" "
		L"--defaults-file=\"" +
		configPath_.wstring() +
		L"\" " +
		SERVICE_NAME;


	SC_HANDLE service = CreateServiceW(
		scm,
		SERVICE_NAME,                  // 服务名
		SERVICE_NAME,                  // 显示名称
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		// 自动启动
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		command.c_str(),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	);


	if (!service)
	{
		DWORD err = GetLastError();

		CloseServiceHandle(scm);
		return false;
	}


	// 设置服务描述
	SERVICE_DESCRIPTIONW desc{};

	std::wstring description = L"本地Mysql服务(Local MySQL Server)";

	desc.lpDescription = description.data();

	ChangeServiceConfig2W(service, SERVICE_CONFIG_DESCRIPTION, &desc);

	// 设置失败自动恢复
	SERVICE_FAILURE_ACTIONSW failure{};

	SC_ACTION actions[3]{};

	actions[0].Type = SC_ACTION_RESTART;
	actions[0].Delay = 5000;

	actions[1].Type = SC_ACTION_RESTART;
	actions[1].Delay = 10000;

	actions[2].Type = SC_ACTION_NONE;

	failure.dwResetPeriod = 86400;
	failure.cActions = 3;
	failure.lpsaActions = actions;

	ChangeServiceConfig2W(service, SERVICE_CONFIG_FAILURE_ACTIONS, &failure);

	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return true;
}

bool MysqlManager::RemoveService()
{
	SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (!scm) {
		wprintf(L"OpenSCManagerW failed, error=%lu\n", GetLastError());
		return false;
	}

	SC_HANDLE service = OpenServiceW(
		scm,
		SERVICE_NAME,
		SERVICE_STOP | DELETE | SERVICE_QUERY_STATUS
	);

	// 服务不存在，当作删除成功
	if (!service) {
		CloseServiceHandle(scm);
		return true;
	}

	// 如果正在运行，先停止
	SERVICE_STATUS_PROCESS status{};
	DWORD bytesNeeded = 0;
	if (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>(&status), sizeof(status), &bytesNeeded))
	{
		if (status.dwCurrentState == SERVICE_RUNNING ||
			status.dwCurrentState == SERVICE_START_PENDING)
		{
			ControlService(service, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&status);

			// 等待停止
			while (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
				reinterpret_cast<LPBYTE>(&status), sizeof(status), &bytesNeeded))
			{
				if (status.dwCurrentState == SERVICE_STOPPED)
					break;
				Sleep(200);
			}

			// 如果还没停，强制杀进程
			if (status.dwCurrentState != SERVICE_STOPPED && status.dwProcessId != 0) {
				HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, status.dwProcessId);
				if (hProc) {
					TerminateProcess(hProc, 1);
					CloseHandle(hProc);
					wprintf(L"Force killed process PID=%lu\n", status.dwProcessId);
				}
			}
		}
	}

	// 删除服务
	BOOL ok = DeleteService(service);

	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return ok == TRUE;
}

bool MysqlManager::IsRunning() const
{
	SC_HANDLE scm = OpenSCManagerW(
		nullptr,
		nullptr,
		SC_MANAGER_CONNECT
	);

	if (!scm)
		return false;


	SC_HANDLE service = OpenServiceW(
		scm,
		SERVICE_NAME,
		SERVICE_QUERY_STATUS
	);

	if (!service)
	{
		CloseServiceHandle(scm);
		return false;
	}


	SERVICE_STATUS_PROCESS status{};
	DWORD size = 0;

	bool result = false;

	if (QueryServiceStatusEx(
		service,
		SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>(&status),
		sizeof(status),
		&size))
	{
		switch (status.dwCurrentState)
		{
		case SERVICE_RUNNING:
		case SERVICE_START_PENDING:
			result = true;
			break;

		default:
			result = false;
			break;
		}
	}


	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return result;
}

bool MysqlManager::Start()
{
	SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (!scm) {
		wprintf(L"OpenSCManagerW failed, error=%lu\n", GetLastError());
		return false;
	}

	SC_HANDLE service = OpenServiceW(
		scm,
		SERVICE_NAME,
		SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG
	);
	if (!service) {
		wprintf(L"OpenServiceW failed, error=%lu\n", GetLastError());
		CloseServiceHandle(scm);
		return false;
	}

	SERVICE_STATUS_PROCESS status{};
	DWORD bytesNeeded = 0;

	// 查询当前状态
	if (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>(&status), sizeof(status), &bytesNeeded))
	{
		if (status.dwCurrentState == SERVICE_RUNNING) {
			wprintf(L"Service already running.\n");
			CloseServiceHandle(service);
			CloseServiceHandle(scm);
			running_ = true;
			return true;
		}
	}

	// 尝试启动服务
	if (!StartServiceW(service, 0, nullptr)) {
		DWORD err = GetLastError();
		if (err != ERROR_SERVICE_ALREADY_RUNNING) {
			wprintf(L"StartServiceW failed, error=%lu\n", err);
			CloseServiceHandle(service);
			CloseServiceHandle(scm);
			return false;
		}
	}

	// 等待服务进入运行状态
	bool ok = false;
	for (;;) {
		if (!QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO,
			reinterpret_cast<LPBYTE>(&status), sizeof(status), &bytesNeeded))
		{
			wprintf(L"QueryServiceStatusEx failed, error=%lu\n", GetLastError());
			break;
		}

		if (status.dwCurrentState == SERVICE_RUNNING) {
			ok = true;
			break;
		}

		if (status.dwCurrentState == SERVICE_STOPPED) {
			wprintf(L"Service stopped. ExitCode=%lu, ServiceSpecific=%lu\n",
				status.dwWin32ExitCode, status.dwServiceSpecificExitCode);
			break;
		}

		if (status.dwCurrentState != SERVICE_START_PENDING) {
			wprintf(L"Unexpected state=%lu\n", status.dwCurrentState);
			break;
		}

		Sleep(200);
	}

	running_ = ok;

	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return ok;
}

bool MysqlManager::Stop() {
	SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (!scm) return false;

	SC_HANDLE service = OpenServiceW(scm, SERVICE_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (!service) {
		CloseServiceHandle(scm);
		return false;
	}

	SERVICE_STATUS status{};
	if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
		while (QueryServiceStatus(service, &status)) {
			if (status.dwCurrentState == SERVICE_STOPPED) break;
			Sleep(200);
		}
	}

	CloseServiceHandle(service);
	CloseServiceHandle(scm);
	return status.dwCurrentState == SERVICE_STOPPED;
};

bool MysqlManager::RunCommand(const std::wstring& command) {
	return true;
}

bool MysqlManager::StartTerminal()
{
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	HANDLE outputRead;
	HANDLE outputWrite;

	CreatePipe(&outputRead, &outputWrite, &sa, 0);
	SetHandleInformation(outputRead, HANDLE_FLAG_INHERIT, 0);

	HANDLE inputRead;
	HANDLE inputWrite;

	CreatePipe(&inputRead, &inputWrite, &sa, 0);
	SetHandleInformation(inputWrite, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOW si{};
	si.cb = sizeof(si);

	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = inputRead;
	si.hStdOutput = outputWrite;
	si.hStdError = outputWrite;

	PROCESS_INFORMATION pi{};
	std::wstring cmd = L"cmd.exe";

	if (!CreateProcessW(
		nullptr,
		cmd.data(),
		nullptr,
		nullptr,
		TRUE,
		CREATE_NO_WINDOW,
		nullptr,
		mysqlPath_.c_str(),
		&si,
		&pi))
	{
		return false;
	}

	CloseHandle(inputRead);
	CloseHandle(outputWrite);
	hProcess_ = pi.hProcess;
	hInputWrite_ = inputWrite;
	hOutputRead_ = outputRead;
	CloseHandle(pi.hThread);
	return true;
}