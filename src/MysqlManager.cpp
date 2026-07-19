#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <miniz.h>
#include <fstream>
#include "head/MysqlManager.h"
#include "head/AppLication.h"

#pragma comment(lib, "Advapi32.lib")

namespace fs = std::filesystem;

MysqlManager::MysqlManager(const Application& app) :m_app(app) {};
MysqlManager::~MysqlManager() = default;

bool MysqlManager::Init() {
	wchar_t exePath[MAX_PATH]{};
	GetModuleFileNameW(nullptr, exePath, MAX_PATH);
	exePath_ = std::filesystem::path(exePath).parent_path();
	resourcePath_ = exePath_ / L"resources";

	wchar_t localAppData[MAX_PATH]{};
	if (FAILED(SHGetFolderPathW(
		nullptr,
		CSIDL_LOCAL_APPDATA,
		nullptr,
		SHGFP_TYPE_CURRENT,
		localAppData)))
	{
		return false;
	}

	rootPath_ = std::filesystem::path(localAppData) / L"LocalMysql";
	mysqlPath_ = rootPath_ / L"mysql";
	dataPath_ = mysqlPath_ / L"data";
	configPath_ = mysqlPath_ / L"my.ini";

	// 创建工作目录
	std::filesystem::create_directories(rootPath_);
	// 判断是否已安装
	installed_ = std::filesystem::exists(mysqlPath_ / L"bin" / L"mysqld.exe");

	return true;
}

bool MysqlManager::Install() {
	if (installed_)
		return true;

	fs::path zip = resourcePath_ / L"mysql-8.4.10-winx64.zip";

	if (!fs::exists(zip))
		return false;

	// 创建安装目录
	fs::create_directories(rootPath_);

	// 解压
	if (!ExtractZip())
		return false;

	// 检查是否安装成功
	installed_ = fs::exists(mysqlPath_ / L"bin" / L"mysqld.exe");

	return installed_;
}

bool MysqlManager::IsInstalled() const {
	return installed_;
}

bool MysqlManager::IsInitialized() const {
	return initialized_;
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
		out << "basedir=" << mysqlPath_.string() << "\n";
		out << "datadir=" << dataPath_.string() << "\n";
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
	if (initialized_)
		return true;

	namespace fs = std::filesystem;

	fs::path mysqld = mysqlPath_ / L"bin" / L"mysqld.exe";

	if (!fs::exists(mysqld))
		return false;

	if (!fs::exists(configPath_))
	{
		if (!CreateConfig())
			return false;
	}

	//
	// 已初始化判断
	//
	if (!fs::exists(dataPath_ / L"mysql.ibd") &&
		!fs::exists(dataPath_ / L"ibdata1"))
	{
		std::wstring cmd =
			L"\"" + mysqld.wstring() +
			L"\" --defaults-file=\"" +
			configPath_.wstring() +
			L"\" --initialize-insecure";

		STARTUPINFOW si{};
		si.cb = sizeof(si);

		PROCESS_INFORMATION pi{};

		std::wstring command = cmd;

		if (!CreateProcessW(
			nullptr,
			command.data(),
			nullptr,
			nullptr,
			FALSE,
			CREATE_NO_WINDOW,
			nullptr,
			mysqlPath_.wstring().c_str(),
			&si,
			&pi))
		{
			return false;
		}

		WaitForSingleObject(pi.hProcess, INFINITE);

		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		if (exitCode != 0)
			return false;
	}


	initialized_ = true;

	return true;
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
		L"LocalMysql",
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

bool MysqlManager::InstallService() {

	std::filesystem::path mysqld = mysqlPath_ / L"bin" / L"mysqld.exe";

	if (!std::filesystem::exists(mysqld))
		return false;

	SC_HANDLE scm = OpenSCManagerW(
		nullptr,
		nullptr,
		SC_MANAGER_CREATE_SERVICE);

	if (!scm)
		//DWORD err = GetLastError();
	return false;

	// 服务已存在
	SC_HANDLE service = OpenServiceW(
		scm,
		L"LocalMysql",
		SERVICE_QUERY_STATUS);

	if (service)
	{
		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return true;
	}

	std::wstring command =
		L"\"" + mysqld.wstring() +
		L"\" --defaults-file=\"" +
		configPath_.wstring() +
		L"\"";

	service = CreateServiceW(
		scm,

		// Service Name
		L"LocalMysql",

		// Display Name
		L"LocalMysql",

		SERVICE_ALL_ACCESS,

		SERVICE_WIN32_OWN_PROCESS,

		SERVICE_AUTO_START,

		SERVICE_ERROR_NORMAL,

		command.c_str(),

		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr);

	if (!service)
	{
		CloseServiceHandle(scm);
		return false;
	}

	// 可选：设置服务描述
	SERVICE_DESCRIPTIONW desc{};
	desc.lpDescription =
		const_cast<LPWSTR>(L"本地Mysql:Local MySQL Server");

	ChangeServiceConfig2W(
		service,
		SERVICE_CONFIG_DESCRIPTION,
		&desc);

	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return true;
};

bool MysqlManager::StopService() {
	SC_HANDLE scm = OpenSCManagerW(
		nullptr,
		nullptr,
		SC_MANAGER_CONNECT);

	if (!scm)
		return false;

	SC_HANDLE service = OpenServiceW(
		scm,
		L"LocalMysql",
		SERVICE_START | SERVICE_QUERY_STATUS);

	if (!service)
	{
		CloseServiceHandle(scm);
		return false;
	}

	SERVICE_STATUS_PROCESS status{};
	DWORD bytesNeeded = 0;

	if (QueryServiceStatusEx(
		service,
		SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>(&status),
		sizeof(status),
		&bytesNeeded))
	{
		if (status.dwCurrentState == SERVICE_RUNNING)
		{
			CloseServiceHandle(service);
			CloseServiceHandle(scm);
			return true;
		}
	}

	BOOL ok = ::StartServiceW(service, 0, nullptr);

	if (!ok)
	{
		DWORD err = GetLastError();

		// 已经在运行
		if (err != ERROR_SERVICE_ALREADY_RUNNING)
		{
			CloseServiceHandle(service);
			CloseServiceHandle(scm);
			return false;
		}
	}

	CloseServiceHandle(service);
	CloseServiceHandle(scm);

	return true;
};

bool MysqlManager::StartService() {
	return false;

};
bool MysqlManager::RemoveService() {
	return false;
};