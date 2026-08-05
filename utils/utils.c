#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static void say_hello()
{
	printf("hello utils\n");
}

static bool open_mysql_terminal(const wchar_t* mysqlExePath, const char* argsUtf8)
{
	// UTF8 -> UTF16
	int needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		argsUtf8,
		-1,
		NULL,
		0
	);


	if (needed <= 0)
		return false;

	wchar_t* argsBuf = (wchar_t*)malloc(sizeof(wchar_t) * needed);

	if (!argsBuf)
		return false;

	MultiByteToWideChar(
		CP_UTF8,
		0,
		argsUtf8,
		-1,
		argsBuf,
		needed
	);

	size_t cmdLen =
		wcslen(mysqlExePath)
		+ wcslen(argsBuf)
		+ 64;


	wchar_t* cmdLine =
		(wchar_t*)malloc(sizeof(wchar_t) * cmdLen);


	if (!cmdLine)
	{
		free(argsBuf);
		return false;
	}


	swprintf(
		cmdLine,
		cmdLen,
		L"powershell.exe -NoExit -Command \"& '%s' %s\"",
		mysqlExePath,
		argsBuf
	);

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(si);
	BOOL result = CreateProcessW(
		NULL,
		cmdLine,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
	);

	free(argsBuf);
	free(cmdLine);

	if (!result)
	{
		return false;
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return true;
}

static bool execute_process(const wchar_t* exePath, const char* argsUtf8, bool showWindow)
{
	if (!exePath)
		return false;

	int argsLen = 0;

	if (argsUtf8 && argsUtf8[0] != '\0')
	{
		argsLen = MultiByteToWideChar(
			CP_UTF8,
			0,
			argsUtf8,
			-1,
			NULL,
			0
		);
	}


	wchar_t* args = NULL;


	if (argsLen > 0)
	{
		args = (wchar_t*)calloc(
			argsLen,
			sizeof(wchar_t)
		);
		if (!args)
			return false;

		MultiByteToWideChar(
			CP_UTF8,
			0,
			argsUtf8,
			-1,
			args,
			argsLen
		);
	}
	else
	{
		args = (wchar_t*)calloc(1, sizeof(wchar_t));
	}

	// 创建完整命令行
	size_t cmdLen = wcslen(exePath) + wcslen(args) + 32;
	wchar_t* cmd = (wchar_t*)calloc(cmdLen, sizeof(wchar_t));

	if (!cmd)
	{
		free(args);
		return false;
	}

	swprintf_s(
		cmd,
		cmdLen,
		L"\"%s\" %s",
		exePath,
		args
	);

	// 调试输出
	wprintf(L"Execute CMD:\n%ls\n", cmd);

	STARTUPINFOW si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	DWORD flags = 0;
	if (!showWindow)
	{
		flags |= CREATE_NO_WINDOW;
	}

	BOOL result = CreateProcessW(
		NULL,
		cmd,            // 必须可写
		NULL,
		NULL,
		FALSE,
		flags,
		NULL,
		NULL,
		&si,
		&pi
	);
	free(args);
	free(cmd);

	if (!result)
	{
		DWORD err = GetLastError();

		printf(
			"CreateProcessW failed: %lu\n",
			err
		);

		return false;
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

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return exitCode == 0;
}

Utils utils =
{
	say_hello,
	open_mysql_terminal,
	execute_process
};