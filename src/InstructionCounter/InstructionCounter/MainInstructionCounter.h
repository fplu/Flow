#ifndef _MAIN_INSTRUCTION_COUNTER_H_
#define _MAIN_INSTRUCTION_COUNTER_H_

#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <time.h>
#include <psapi.h>


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



#ifdef INSTRUCTIONCOUNTER_EXPORTS
	#define INSTRUCTIONCOUNTER_LIB __declspec(dllexport)
#else
	#define INSTRUCTIONCOUNTER_LIB __declspec(dllimport)
#endif


#include "Macro.h"
#include "LIST_BASIC.h"
#include "GRAPH_BASIC.h"

#include "Miscallenious.h"

#include "Opcode.h"
#include "ModRM.h"
#include "MemoryAccess.h"

#include "Jcc.h"
#include "JmpAndCall.h"
#include "Ret.h"
#include "Branch.h"

#include "InstructionCounter.h"
#include "Prefix.h"
#include "MovOpcode.h"

#include "InstructionReadExport.h"

#ifdef _DEBUG
	#pragma comment(lib, "../x64/Debug/LIST_AND_GRAPH.lib")
#else
	#pragma comment(lib, "../x64/Release/LIST_AND_GRAPH.lib")
#endif // _DEBUG

#endif