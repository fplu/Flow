#ifndef _MAIN_H_
#define _MAIN_H_

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <time.h>
#include <Psapi.h>

#define _Thread_local __declspec(thread) 

#ifndef MSDN_FUNC_ERROR
#ifdef UNICODE
	#define MSDN_FUNC_ERROR() \
		_tprintf(_T("error on line %d, in file %s, in func %S, error code %d\n"), __LINE__, _T(__FILE__),__func__, GetLastError()); \
		__leave;
#else
	#define MSDN_FUNC_ERROR() \
		_tprintf(_T("error on line %d, in file %s, in func %s, error code %d\n"), __LINE__, _T(__FILE__), __func__, GetLastError()); \
		__leave;
#endif
#endif // !MSDN_FUNC_ERROR

#include "MainINSTRUCTIONCounter.h"
#include "Hook.h"
#include "FlowMain.h"

#include "dllmain.h"
#include "InjectorMain.h"

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/FlowUser.lib")
#pragma comment(lib, "../x64/Debug/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Debug/Injector.lib")
#pragma comment(lib, "../x64/Debug/Flow.lib")
#pragma comment(lib, "../x64/Debug/Hook.lib")
#else
#pragma comment(lib, "../x64/Release/FlowUser.lib")
#pragma comment(lib, "../x64/Release/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Release/Injector.lib")
#pragma comment(lib, "../x64/Release/Flow.lib")
#pragma comment(lib, "../x64/Release/Hook.lib")
#endif // _DEBUG

#endif