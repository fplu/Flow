#ifndef _INSTRUCTION_POINTER_MANAGEMENT_H_
#define _INSTRUCTION_POINTER_MANAGEMENT_H_

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>


#ifdef _LIB_INC_RIP_
#define _LIB_INC_RIP_API_ __declspec(dllexport)
#else
#define _LIB_INC_RIP_API_ __declspec(dllimport)
#endif


#include "MainINSTRUCTIONCounter.h"
#include "Hook.h"
#include "InjectorMain.h"



#include "LdrRegisterDllNotification.h"
#include "ModuleMapping.h"

#include "FlowInit.h"

#include "SingleStepHandler.h"
#include "IncRipCache.h"
#include "SyscallHandler.h"
#include "ExceptionHandler.h"
#include "ThreadCreationHandler.h"

#include "Callback.h"


#include "ExitMapping.h"
#include "Jmp.h"
#include "Call.h"
#include "Jcc.h"
#include "Loop.h"
#include "Syscall.h"
#include "Ret.h"
#include "RipRel.h"
#include "Others.h"

#include "FlowExport.h"

#include "Injecteur.h"

_LIB_INC_RIP_API_ extern void * TAKE_THIS();

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Debug/Hook.lib")
#pragma comment(lib, "../x64/Debug/Injector.lib")
#else
#pragma comment(lib, "../x64/Release/INSTRUCTIONCounter.lib")
#pragma comment(lib, "../x64/Release/Hook.lib")
#pragma comment(lib, "../x64/Release/Injector.lib")
#endif // _DEBUG


#endif // !_INSTRUCTION_POINTER_MANAGEMENT_H_

