#include "FlowMain.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

extern OPCODE START_INJECTOR_;
extern OPCODE END_INJECTOR_;

extern QWORD RetAddress_;
extern QWORD GetModuleHandle_;
extern QWORD GetProcAddress_;
//extern QWORD Beep_;
extern QWORD LoadLibrary_;

extern DWORD DLL_PATHS_LENGTH;
extern TCHAR DLL_PATHS;
//TCHAR DLL1_PATH;
//TCHAR DLL2_PATH;
//TCHAR DLL2_NAME;

/*
	Can inject up to 10 dll, no more
*/
BOOL injectEmulatorAndUserDll(_In_ const TCHAR * const * const dllNames, _In_ const DWORD dllNamesLength, _In_ const TCHAR * const exePath, _In_opt_ TCHAR * exeCommandLine) {
	BOOL success = TRUE;
	TCHAR ** fullDllNames = NULL;
	int i;
	TCHAR exeInjectorPath[500] = { 0 };
	PROCESS_INFORMATION processInformation;
	BOOL is32BitProcess;

	__try {
		if (dllNames == NULL || dllNamesLength == 0 || exePath == NULL) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if ((fullDllNames = calloc(dllNamesLength, sizeof(TCHAR*))) == NULL) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		GetModuleFileName(NULL, exeInjectorPath, MAX_PATH);
		for (i = 0; exeInjectorPath[i] != _T('\0'); i++);

		for (i--; i >= 0; i--) {
			if (exeInjectorPath[i] == _T('\\') || exeInjectorPath[i] == _T('/')) {
				exeInjectorPath[i + 1] = _T('\0');
				break;
			}
		}




		for (i = 0; i < (int)dllNamesLength; i++) {

			if ((fullDllNames[i] = malloc(500 * sizeof(TCHAR*))) == NULL) {
				success = FALSE;
				MSDN_FUNC_ERROR();
			}

			if (PathCombine(fullDllNames[i], exeInjectorPath, dllNames[i]) == NULL) {
				success = FALSE;
				MSDN_FUNC_ERROR();
			}
		}

		ZeroMemory(&processInformation, sizeof(PROCESS_INFORMATION));
		OpenProcessForHook(exePath, exeCommandLine, &is32BitProcess, &processInformation);

		//__debugbreak();

		/*/
			CloseHandle(processInformation.hThread);
			CloseHandle(processInformation.hProcess);
			processInformation.hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processInformation.dwProcessId);
			processInformation.hThread = OpenThread(THREAD_SET_CONTEXT | THREAD_GET_CONTEXT | THREAD_SET_LIMITED_INFORMATION | THREAD_QUERY_LIMITED_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, processInformation.dwThreadId);
			executeInjectorFunction1(processInformation.hProcess, processInformation.hThread, dllPathIncRipUserTest);
		/*/
			executeInjectorFunction2(processInformation.hProcess, processInformation.hThread, fullDllNames, dllNamesLength);
		/**/
	}
	__finally {

	}


	return success;
}

BOOL prepareInjectorFunction(const CONTEXT * context, _In_ const TCHAR * const * const dllPaths, DWORD dllPathsLenght) {
	BOOL success = TRUE;
	DWORD oldProtect;
	DWORD i;

	__try {
		VirtualProtect(&START_INJECTOR_, &END_INJECTOR_ - &START_INJECTOR_, PAGE_EXECUTE_READWRITE, &oldProtect);

		RetAddress_ = context->Rip;
		GetProcAddress_ = (QWORD)GetProcAddress;
		DLL_PATHS_LENGTH = dllPathsLenght;

#ifdef UNICODE
		GetModuleHandle_ = (QWORD)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetModuleHandleW");
		LoadLibrary_ = (QWORD)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
#else
		GetModuleHandle_ = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetModuleHandleA");
		LoadLibrary_ = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
#endif	
		//Beep_ = (QWORD)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "Beep");

		for (i = 0; i < dllPathsLenght; i++) {
			_tcscpy_s((TCHAR*)((BYTE*)&DLL_PATHS + ((size_t)1000 * (size_t)i)), 500, dllPaths[i]);
		}

		//if (!GetFullPathName(DLL64_1, 500 * sizeof(TCHAR), &DLL1_PATH, NULL)) {
		//	MSDN_FUNC_ERROR();
		//}
		//_tcscpy_s(&DLL2_PATH, 500, dllName);



		VirtualProtect(&START_INJECTOR_, &END_INJECTOR_ - &START_INJECTOR_, PAGE_EXECUTE_READWRITE, &oldProtect);
	}
	__finally {

	}
	return success;
}

BOOL executeInjectorFunction1(HANDLE hProcess, HANDLE hThread, _In_ const TCHAR * const * const dllPaths, _In_ DWORD dllPathsLength) {
	BOOL success = TRUE;
	BYTE * page;
	CONTEXT context;

	__try {
		takeThreadControl(hProcess, hThread, &context);
		ResumeThread(hThread);
		CLR_Wait(hThread, &context);
		prepareInjectorFunction(&context, dllPaths, dllPathsLength);
		myVirtualAllocEx(hThread, &context, (DWORD)(&END_INJECTOR_ - &START_INJECTOR_), &page);
		myWriteProcessMemory(hThread, &context, &START_INJECTOR_, (DWORD)(&END_INJECTOR_ - &START_INJECTOR_), page);
		context.Rip = (DWORD64)page;
		releaseThreadControl(hThread, &context);


	}
	__finally {

	}


	return success;
}

BOOL executeInjectorFunction2(HANDLE hProcess, HANDLE hThread, _In_ const TCHAR * const * const dllPaths, _In_ DWORD dllPathsLength) {
	BOOL success = TRUE;
	BYTE * page;
	SIZE_T numberOfByteWritten;
	CONTEXT context;
	DWORD threadId;
	HMODULE hNtdll;
	FARPROC procGetLastError;
	__try {
		if (hThread == INVALID_HANDLE_VALUE || hThread == NULL) {

			if ((hNtdll = GetModuleHandleA("ntdll")) == NULL) {
				success = FALSE;
				__leave;
			}

			if ((procGetLastError = GetProcAddress(hNtdll, "GetLastError")) == NULL) {
				success = FALSE;
				__leave;
			}

			if ((hThread = CreateRemoteThread(hProcess, NULL, 0, NULL, procGetLastError, CREATE_SUSPENDED, &threadId)) == NULL) {
				success = FALSE;
				__leave;
			}
		}
		/*ResumeThread(hThread);
		Sleep(100);

		DebugBreak();
		SuspendThread(hThread);*/


		context.ContextFlags = CONTEXT_ALL;
		GetThreadContext(hThread, &context);
		prepareInjectorFunction(&context, dllPaths, dllPathsLength);

		if ((page = VirtualAllocEx(hProcess, NULL, &END_INJECTOR_ - &START_INJECTOR_, MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL) {
			success = FALSE;
			__leave;
		}
		WriteProcessMemory(hProcess, page, &START_INJECTOR_, &END_INJECTOR_ - &START_INJECTOR_, &numberOfByteWritten);
		context.ContextFlags = CONTEXT_CONTROL;
		context.Rip = (DWORD64)page;

		SetThreadContext(hThread, &context);
		ResumeThread(hThread);



	}
	__finally {

	}


	return success;
}
