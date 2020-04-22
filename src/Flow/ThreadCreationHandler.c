#include "FlowMain.h"

extern DWORD g_TlsIdDocker;

void ThreadCreationHandlerRun() {
	if (TlsGetValue(g_TlsIdDocker) == 0) {
		saveCallAddress((void*)TAKE_THIS);
	}
}

void ThreadCreationHandlerInit() {
	FARPROC ldrInitializeThunk;
	FARPROC rtlUserThreadStart;

	//__debugbreak();


	ldrInitializeThunk = GetProcAddress(GetModuleHandle(_T("ntdll")), "LdrInitializeThunk");
	rtlUserThreadStart = GetProcAddress(GetModuleHandle(_T("ntdll")), "RtlUserThreadStart");

	/*if (!placeHookTrampoline(ldrInitializeThunk, NULL, ThreadCreationHandlerRun, NULL)) {
		return;
	}

	if (!placeHookTrampoline(rtlUserThreadStart, NULL, ThreadCreationHandlerRun, NULL)) {
		return;
	}*/

	return;
}