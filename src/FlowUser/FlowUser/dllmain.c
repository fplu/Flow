#include "dllmain.h"

BOOL APIENTRY DllMain(_In_ HMODULE hModule,_In_ DWORD  fdwReason,_In_ LPVOID lpReserved) {


	if (fdwReason == DLL_PROCESS_ATTACH) {
	//	ExceptionHandlerInit();
		SyscallHandlerInit();
	}
	else if(fdwReason == DLL_THREAD_ATTACH) {
	}
	else if (fdwReason == DLL_THREAD_DETACH) {
		debugCurrentThread();
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {

	}

    return TRUE;
}


/*
*	If a function is named "processInitializationCallback" in the user dll it will ba called at process initialization outside of a dllMain
*/
void processInitializationCallback() {
	return;
}


/*
*	If a function is named "threadInitializationCallback" in the user dll it will ba called at each time a new thread start to be instrumented
*/
void threadInitializationCallback(EMULATOR_HANDLER * pEmulatorHandler) {
	return;
}

/*
*	If a function is named "getInstructionCallback" in the user dll it will ba called at each time an instruction is instrumented.
*	It take the current instruction as parameter and return a pre callback for this instruction.
*	The pre callback will receive a handle to Flow as first parameter each time it is called
*/
void(*getInstructionCallback(INSTRUCTION_READ * pInstructionRead))(EMULATOR_HANDLER *) {
	return NULL;
}

/*
*	If a function is named "exceptionCallback" it will be called each time an unexpected behaviour is detected passing OPTIONNALY a handle to FLOW or optionnaly a context to the code which cause the exception and the reason of exception
*/
void exceptionCallback(EMULATOR_HANDLER* pEmulatorHandler, CONTEXT* context, DWORD reason) {
	return;
}

/*
*	If a function is named "enableModuleInstructionAnalyse" in the user dll, it should return wether or not we enable all callbacks in a region of memory passed as argument (starting to baseAddress to baseAddress + baseSize).
*	TRUE the callback are enable. FALSE the callback are disable
*	When this function is not set we consider all callback are enable.
*/
BOOL enableModuleInstructionAnalyse(OPCODE* baseAddress, DWORD baseSize) {
	return TRUE;
}




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
BOOL enableRangeInstructionInstrumentation(OPCODE* baseAddress, DWORD baseSize) {
	return TRUE;
}