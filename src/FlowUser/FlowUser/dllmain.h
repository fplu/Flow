#ifndef _EMULATOR_MAIN_H_
#define _EMULATOR_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <time.h>
#include <shlwapi.h>

#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")


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

#ifdef INCRIPTESTUSER_EXPORTS
#define FLOW_USER_LIB __declspec(dllexport)
#else
#define FLOW_USER_LIB __declspec(dllimport)
#endif



#include "MainInstructionCounter.h"
#include "FlowMain.h"


#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Debug/Flow.lib")
#else
#pragma comment(lib, "../x64/Release/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Release/Flow.lib")
#endif // _DEBUG


/*
*	If a function is named "processInitializationCallback" in the user dll it will ba called at process initialization outside of a dllMain
*/
FLOW_USER_LIB void processInitializationCallback();


/*
*	If a function is named "threadInitializationCallback" in the user dll it will ba called at each time a new thread start to be instrumented
*/
FLOW_USER_LIB void threadInitializationCallback(EMULATOR_HANDLER* pEmulatorHandler);

/*
*	If a function is named "getInstructionCallback" in the user dll it will ba called at each time an instruction is instrumented.
*	It take the current instruction as parameter and return a pre callback for this instruction.
*	The pre callback will receive a handle to Flow as first parameter each time it is called
*/
FLOW_USER_LIB void(*getInstructionCallback(INSTRUCTION_READ* pInstructionRead))(EMULATOR_HANDLER*);

/*
*	If a function is named "exceptionCallback" it will be called each time an unexpected behaviour is detected passing OPTIONNALY a handle to FLOW or optionnaly a context to the code which cause the exception and the reason of exception
*/
FLOW_USER_LIB void exceptionCallback(EMULATOR_HANDLER* pEmulatorHandler, CONTEXT* context, DWORD reason);

/*
*	If a function is named "enableModuleInstructionAnalyse" in the user dll, it should return wether or not we enable all callbacks in a region of memory passed as argument (starting to baseAddress to baseAddress + baseSize).
*	TRUE the callback are enable. FALSE the callback are disable
*	When this function is not set we consider all callback are enable.
*/
FLOW_USER_LIB BOOL enableModuleInstructionAnalyse(OPCODE* baseAddress, DWORD baseSize);

/*
*	If a function is named "enableRangeInstructionInstrumentation" in the user dll, it should return wether or not we instrument the region of code.
*	TRUE we instrument the region.
*	FALSE every calls to the region will not be intrumented. An uninstrumented region cannot be analyzed.
*	The instrumentation will restart when the first function called in the region return.
*	Disabling instrumentation in a region of code allow a gain of speed (especially with mscore when instrumenting .NET	application) and a gain of RAM.
*	When this function is not set we consider that everything should be instrumented.
*
*	Nb : In some rare case, when analysing shellcode, disabling instrumentation using this function could cause performance loss
*/
FLOW_USER_LIB BOOL enableRangeInstructionInstrumentation(OPCODE* baseAddress, DWORD baseSize);

#endif

