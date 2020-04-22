#ifndef _INJECTOR_MAIN_H_
#define _INJECTOR_MAIN_H_

#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <tchar.h>

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

typedef unsigned long long QWORD;

#include "../../INSTRUCTIONCounter/INSTRUCTIONCounter/Opcode.h"

#include "HijackRunningThread.h"
#include "InjectionCreateRemoteThread.h"

#endif // !_INJECTOR_MAIN_H_
