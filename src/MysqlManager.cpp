#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <miniz.h>
#include "head/MysqlManager.h"
#include "head/AppLication.h"

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
	installed_ = fs::exists(
		mysqlPath_ / L"bin" / L"mysqld.exe");

	return installed_;
}

bool MysqlManager::IsInstalled() const {
	return installed_;
}

bool MysqlManager::ExtractZip() const
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