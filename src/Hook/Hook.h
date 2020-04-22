#ifndef _MAIN_H_
#define _MAIN_H_



#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#ifdef HOOK_EXPORTS
#define HOOK_LIB __declspec(dllexport)
#else
#define HOOK_LIB __declspec(dllimport)
#endif

#include "MainINSTRUCTIONCounter.h"


#include "ListeThreadInformation.h"
#include "ListeHook.h"
#include "HookTrampoline.h"

#define TPRINTF_ERROR(text) \
	_tprintf(text);\
	success = FALSE;\
	__leave;

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Debug/LIST_AND_GRAPH.lib")
#else
#pragma comment(lib, "../x64/Release/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Release/LIST_AND_GRAPH.lib")
#endif // _DEBUG




#endif