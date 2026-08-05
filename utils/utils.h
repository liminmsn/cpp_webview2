#pragma once

#include <stdbool.h>
#include <wchar.h>


#ifdef __cplusplus
extern "C" {
#endif


	typedef struct Utils
	{
		void (*say_hello)();
		// 打开mysql本地终端
		bool (*open_mysql_terminal)(
			const wchar_t* mysqlExePath,
			const char* argsUtf8
			);

		// 通用执行程序
		bool (*execute_process)(
			const wchar_t* exePath,
			const char* argsUtf8,
			bool showWindow
			);
	} Utils;


	extern Utils utils;


#ifdef __cplusplus
}
#endif