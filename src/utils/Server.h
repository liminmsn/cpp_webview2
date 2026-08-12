#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#include <functional>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

struct ServerCheckPortUsage
{
	std::vector<DWORD> Props; // 保存占用该端口的进程 PID
	std::string Info;         // 保存描述信息
};

inline ServerCheckPortUsage CheckPortUsage(int port)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	ServerCheckPortUsage result;

	PMIB_TCPTABLE_OWNER_PID pTcpTable = nullptr;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	// 第一次调用获取所需缓冲区大小
	GetExtendedTcpTable(nullptr, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
	pTcpTable = (PMIB_TCPTABLE_OWNER_PID)malloc(dwSize);

	if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == NO_ERROR)
	{
		std::ostringstream oss;
		for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++)
		{
			MIB_TCPROW_OWNER_PID row = pTcpTable->table[i];
			int localPort = ntohs((u_short)row.dwLocalPort);

			if (localPort == port)
			{
				result.Props.push_back(row.dwOwningPid);
				oss << "Port " << port << " is used by PID=" << row.dwOwningPid << "\n";
			}
		}
		result.Info = oss.str();
	}
	else
	{
		std::ostringstream oss;
		oss << "GetExtendedTcpTable failed, error: " << dwRetVal;
		result.Info = oss.str();
	}

	if (pTcpTable) free(pTcpTable);

	WSACleanup();
	return result;
}

inline bool KillPort(int port)
{
	ServerCheckPortUsage usage = CheckPortUsage(port);
	if (usage.Props.empty())
	{
		std::cout << "Port " << port << " is not in use." << std::endl;
		return false;
	}

	for (auto pid : usage.Props)
	{
		HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
		if (hProcess)
		{
			if (TerminateProcess(hProcess, 0))
			{
				std::cout << "Killed process PID=" << pid << " using port " << port << std::endl;
			}
			else
			{
				std::cerr << "Failed to kill PID=" << pid << ", error=" << GetLastError() << std::endl;
			}
			CloseHandle(hProcess);
		}
		else
		{
			std::cerr << "Cannot open process PID=" << pid << ", error=" << GetLastError() << std::endl;
		}
	}
	return true;
}

inline void RunCommandWithOutput(
	const std::string& cmd,
	std::function<void(const std::string&)> onOutput,
	const std::string& workDir = ""
)
{
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;

	HANDLE hReadPipe = nullptr;
	HANDLE hWritePipe = nullptr;


	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
	{
		onOutput("CreatePipe failed error=" + std::to_string(GetLastError()));
		return;
	}

	SetHandleInformation(
		hReadPipe,
		HANDLE_FLAG_INHERIT,
		0
	);

	STARTUPINFOA si{};
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESTDHANDLES;

	si.hStdOutput = hWritePipe;
	si.hStdError = hWritePipe;
	si.hStdInput = NULL;

	PROCESS_INFORMATION pi{};
	std::string command = cmd;
	BOOL ret = CreateProcessA(
		nullptr,
		command.data(),
		nullptr,
		nullptr,
		TRUE,
		CREATE_NO_WINDOW,
		nullptr,

		// 关键修改
		workDir.empty() ? nullptr : workDir.c_str(),

		&si,
		&pi
	);


	CloseHandle(hWritePipe);


	if (!ret)
	{
		onOutput(
			"CreateProcess failed error="
			+ std::to_string(GetLastError())
		);

		CloseHandle(hReadPipe);
		return;
	}


	char buffer[4096]{};
	DWORD read = 0;


	while (ReadFile(
		hReadPipe,
		buffer,
		sizeof(buffer) - 1,
		&read,
		nullptr))
	{
		if (read == 0)
			break;

		buffer[read] = 0;

		onOutput(buffer);
	}
	WaitForSingleObject(
		pi.hProcess,
		INFINITE
	);
	DWORD exitCode = 0;
	GetExitCodeProcess(
		pi.hProcess,
		&exitCode
	);

	onOutput(
		"[exit code="
		+ std::to_string(exitCode)
		+ "]"
	);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hReadPipe);
}

inline bool StartProcessAndStreamOutput(
	const std::string& cmd,
	std::function<void(const std::string&)> onOutput,
	PROCESS_INFORMATION* outPi = nullptr,
	const std::string& workDir = "" // 如果为空，使用父进程当前目录
)
{
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;

	HANDLE hReadPipe = nullptr;
	HANDLE hWritePipe = nullptr;
	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
		try { onOutput("CreatePipe failed error=" + std::to_string(GetLastError())); }
		catch (...) {}
		return false;
	}

	if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0)) {
		try { onOutput("SetHandleInformation failed error=" + std::to_string(GetLastError())); }
		catch (...) {}
		CloseHandle(hReadPipe);
		CloseHandle(hWritePipe);
		return false;
	}

	STARTUPINFOA si{};
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdOutput = hWritePipe;
	si.hStdError = hWritePipe;
	si.hStdInput = NULL;

	PROCESS_INFORMATION pi{};
	std::vector<char> cmdBuf(cmd.begin(), cmd.end());
	cmdBuf.push_back('\0');

	// 使用 CREATE_NEW_PROCESS_GROUP 以便后续可用 GenerateConsoleCtrlEvent 发送 Ctrl+C
	DWORD dwCreationFlags = CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP;

	BOOL ret = CreateProcessA(
		nullptr,
		cmdBuf.data(),
		nullptr,
		nullptr,
		TRUE,
		dwCreationFlags,
		nullptr,
		workDir.empty() ? nullptr : workDir.c_str(), // 关键：设置子进程工作目录
		&si,
		&pi
	);

	CloseHandle(hWritePipe);
	hWritePipe = nullptr;

	if (!ret) {
		try { onOutput("CreateProcess failed error=" + std::to_string(GetLastError())); }
		catch (...) {}
		CloseHandle(hReadPipe);
		return false;
	}

	std::thread reader([hReadPipe, onOutput]() mutable {
		constexpr DWORD BUF_SZ = 4096;
		std::vector<char> buffer(BUF_SZ);
		DWORD read = 0;
		while (true) {
			BOOL ok = ReadFile(hReadPipe, buffer.data(), BUF_SZ, &read, nullptr);
			if (!ok) {
				DWORD err = GetLastError();
				if (err == ERROR_BROKEN_PIPE) break;
				try { onOutput("ReadFile failed error=" + std::to_string(err)); }
				catch (...) {}
				break;
			}
			if (read == 0) break;
			try { onOutput(std::string(buffer.data(), read)); }
			catch (...) { break; }
		}
		CloseHandle(hReadPipe);
		});
	reader.detach();

	if (outPi) {
		*outPi = pi; // 调用者负责 CloseHandle(pi.hProcess) 和 CloseHandle(pi.hThread)
	}
	else {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	return true;
}

inline bool StopChildProcess(PROCESS_INFORMATION* pi, DWORD graceMs = 3000, DWORD* outExitCode = nullptr)
{
	if (!pi || pi->hProcess == nullptr) return false;

	DWORD exitCode = 0;
	if (!GetExitCodeProcess(pi->hProcess, &exitCode)) return false;
	if (exitCode != STILL_ACTIVE) {
		if (outExitCode) *outExitCode = exitCode;
		return true;
	}

	// 先尝试发送 Ctrl+C 到子进程组（需要 CREATE_NEW_PROCESS_GROUP）
	if (GenerateConsoleCtrlEvent(CTRL_C_EVENT, pi->dwProcessId)) {
		DWORD waitRes = WaitForSingleObject(pi->hProcess, graceMs);
		if (waitRes == WAIT_OBJECT_0) {
			GetExitCodeProcess(pi->hProcess, &exitCode);
			if (outExitCode) *outExitCode = exitCode;
			return true;
		}
	}

	// 回退到强制终止
	if (!TerminateProcess(pi->hProcess, 1)) return false;
	WaitForSingleObject(pi->hProcess, INFINITE);
	GetExitCodeProcess(pi->hProcess, &exitCode);
	if (outExitCode) *outExitCode = exitCode;
	return true;
}