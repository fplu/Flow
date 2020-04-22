#include "Hook.h"

#define PIPENAME L"\\\\.\\pipe\\UnPipe"
#define PIPESIZE 1000

extern DWORD tlsIndex;
INSTRUCTION_TREE * g_Tree = NULL;
char * g_OpcodeBase = NULL;

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD     fdwReason, _In_ LPVOID    lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		initHook();
	}
	else if (fdwReason == DLL_THREAD_ATTACH) {
	}
	else if (fdwReason == DLL_THREAD_DETACH) {
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {

	}
	return TRUE;
}

