#include "MainINSTRUCTIONCounter.h"


extern TCHAR resourceDllPath[500];



BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH) {
		CONST TCHAR * dllResource = RESOURCE_OPCODE;
		int j, i;

		//INIT resourceDllPath
		j = GetModuleFileName(hinstDLL, resourceDllPath, 500);
		for (; resourceDllPath[j] != _T('\\'); j--);
		for (i = 0; dllResource[i] != _T('\0'); i++)
			resourceDllPath[j + i] = dllResource[i];
		resourceDllPath[j + i] = _T('\0');


		//INIT abre INSTRUCTION
		if (!CreateEntireInstructionTree()) {
			return FALSE;
		}
	}
	if (fdwReason == DLL_THREAD_ATTACH) {
	}
	if (fdwReason == DLL_PROCESS_DETACH) {
	}
	return TRUE;
}
