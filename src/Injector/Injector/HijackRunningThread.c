#include "InjectorMain.h"

extern unsigned char START_CALL_INSIDE_THREAD;
extern unsigned char MOV_RAX_CALL;
extern unsigned char MOV_RCX_ARG1;
extern unsigned char MOV_RDX_ARG2;
extern unsigned char END_CALL_INSIDE_THREAD;

extern unsigned char START_REMOTE_GET_PROC_ADDRESS;
extern unsigned char MOV_RAX_GetModuleHandleA;
extern unsigned char MOV_RAX_GetProcAddress;
extern unsigned char MOV_RCX_START_REMOTE_GET_PROC_ADDRESS;
extern unsigned char END_REMOTE_GET_PROC_ADDRESS;

static void * remoteRip = NULL;


BOOL initRemoteRip() {
	BOOL success = TRUE;
	MODULEINFO moduleInfo;

	__try {
		if (remoteRip != NULL) {
			__leave;
		}
		if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA("ntdll"), &moduleInfo, sizeof(MODULEINFO))) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		for (DWORD i = 0; i < moduleInfo.SizeOfImage - 3; i++) {
			if (*(DWORD*)((BYTE*)moduleInfo.lpBaseOfDll + i) == 0xFFFEEB53) {
				remoteRip = (DWORD*)((BYTE*)moduleInfo.lpBaseOfDll + i);
				break;
			}
		}
		if (remoteRip == NULL) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
	}
	__finally {

	}
	return success;
}


BOOL synchronizeThread(_In_ HANDLE hRemoteProcess, _In_ HANDLE hRemoteThread) {
	BOOL success = TRUE;
	HANDLE hCurrentThread;
	DWORD_PTR processAffinityMask;
	DWORD_PTR systemAffinityMask;
	DWORD_PTR threadAffinityMask;
	LONG64 base;

	__try {
		hCurrentThread = GetCurrentThread();

		//STEP 1, get process affinity mask, to use a valid mask for set thread affinity mask
		if (!GetProcessAffinityMask(hRemoteProcess, &processAffinityMask, &systemAffinityMask)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		threadAffinityMask = 0;
		_BitScanForward64((DWORD*)&threadAffinityMask, processAffinityMask);
		_bittestandset64(&base, threadAffinityMask);
		threadAffinityMask = base;


		if (!SetThreadAffinityMask(hCurrentThread, threadAffinityMask)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		if (!SetThreadAffinityMask(hRemoteThread, threadAffinityMask)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

	}
	__finally {

	}

	return success;
}

BOOL takeThreadControl(_In_ HANDLE hRemoteProcess, _In_ HANDLE hRemoteThread, _Outptr_ CONTEXT * pInitialContext) {
	BOOL success = TRUE;
	DWORD64 Rip;

	__try {
		if (!initRemoteRip()) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		if (!synchronizeThread(hRemoteProcess, hRemoteThread)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		pInitialContext->ContextFlags = CONTEXT_ALL;

		SwitchToThread();
		if (!GetThreadContext(hRemoteThread, pInitialContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		Rip = pInitialContext->Rip;
		pInitialContext->Rip = (DWORD64)remoteRip + 1;
		if (!SetThreadContext(hRemoteThread, pInitialContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		pInitialContext->Rip = Rip;
	}
	__finally {

	}
	return success;
}

BOOL CLR_Wait(_In_ HANDLE hRemoteThread, _In_  CONTEXT * pInitialContext) {
	BOOL success = FALSE;
	CONTEXT injectedContext;

	__try {

		injectedContext.ContextFlags = CONTEXT_FULL;
		Sleep(100);
		
		do {
			SwitchToThread();
			if (!GetThreadContext(hRemoteThread, &injectedContext)) {
				success = FALSE;
				MSDN_FUNC_ERROR();
			}
		} while (!(injectedContext.Rip == (QWORD)remoteRip + 1));

		injectedContext.Rip = pInitialContext->Rip;
		*pInitialContext = injectedContext;
	}
	__finally {

	}

	return success;
}

BOOL releaseThreadControl(_In_ HANDLE hRemoteThread, _In_  CONTEXT * pInitialContext) {
	BOOL success = TRUE;
	__try {
		SwitchToThread();
		if (!SetThreadContext(hRemoteThread, pInitialContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
	}
	__finally {

	}
	return success;
}

BOOL remoteCall(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ void * function, _In_opt_ void * arg1, _In_opt_ void * arg2, _In_opt_ void * arg3, _In_opt_ void * arg4, _Outptr_ void ** retValue) {
	BOOL success = TRUE;

	CONTEXT injectedContext;
	__try {

		//STEP 1.1 push ret address
		//STEP 1.1.1 Mov execution
		injectedContext = *pInitialContext;
		injectedContext.ContextFlags = CONTEXT_FULL;
		injectedContext.Rbx = (DWORD64)remoteRip + 1;
		injectedContext.Rip = (DWORD64)remoteRip;
		injectedContext.Rsp += 0x30;
		injectedContext.Rsp &= ~0x7;
		injectedContext.Rsp += 0x08;

		SwitchToThread();
		if (!SetThreadContext(hRemoteThread, &injectedContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		//STEP 1.1.2 Wait for execution
		injectedContext.ContextFlags = CONTEXT_CONTROL;
		do {
			SwitchToThread();
			if (!GetThreadContext(hRemoteThread, &injectedContext)) {
				success = FALSE;
				MSDN_FUNC_ERROR();
			}
		} while (!(injectedContext.Rip == (QWORD)remoteRip + 1));

		//STEP 1.2 call function
		//STEP 1.2.1 passing argument and calling function
		injectedContext = *pInitialContext;
		injectedContext.Rip = (DWORD64)function;
		injectedContext.Rcx = (DWORD64)arg1;
		injectedContext.Rdx = (DWORD64)arg2;
		injectedContext.R8 = (DWORD64)arg3;
		injectedContext.R9 = (DWORD64)arg4;
		injectedContext.Rsp += 0x30;
		injectedContext.Rsp &= ~0x7;
		injectedContext.Rsp += 0x08;
		injectedContext.Rsp -= 0x8;
		SwitchToThread();
		if (!SetThreadContext(hRemoteThread, &injectedContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		//STEP 1.2.2 Wait for execution
		SwitchToThread();
		injectedContext.ContextFlags = CONTEXT_CONTROL;
		do {
			if (!GetThreadContext(hRemoteThread, &injectedContext)) {
				success = FALSE;
				MSDN_FUNC_ERROR();
			}
		} while (!(injectedContext.Rip == (QWORD)remoteRip + 1));


		//STEP 1.3 get ret value
		injectedContext.ContextFlags = CONTEXT_FULL;
		if (!GetThreadContext(hRemoteThread, &injectedContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		*retValue = (void*)injectedContext.Rax;

	}
	__finally {
	}

	return success;
}

BOOL myVirtualAllocEx(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ DWORD bufferSize, _Out_ BYTE ** pRemoteBuffer) {
	BOOL success= TRUE;
	void * virtualAllocAddress;

	__try {
		if ((virtualAllocAddress = GetProcAddress(GetModuleHandleA("Kernel32"), "VirtualAlloc")) == NULL) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		if (!remoteCall(hRemoteThread, pInitialContext, virtualAllocAddress, 0, (void*)(QWORD)bufferSize, (void*)MEM_COMMIT, (void*)PAGE_EXECUTE_READWRITE, (void**)pRemoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
	}
	__finally {
	}
	return success;
}

BOOL myWriteProcessMemory(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ BYTE * buffer, _In_ DWORD bufferSize, _In_ BYTE * remoteBuffer) {
	BOOL success = TRUE;
	CONTEXT injectedContext;
	DWORD byteWritten;

	__try {
		if (!initRemoteRip()) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}


		injectedContext = *pInitialContext;

		for (byteWritten = 0; byteWritten < bufferSize; byteWritten += 8) {

			injectedContext.ContextFlags = CONTEXT_FULL;
			injectedContext.Rsp = (DWORD64)remoteBuffer + 8 + byteWritten;
			injectedContext.Rbx = *(QWORD*)(buffer + byteWritten);
			injectedContext.Rip = (DWORD64)remoteRip;

			SwitchToThread();
			if (!SetThreadContext(hRemoteThread, &injectedContext)) {
				success = FALSE;
				MSDN_FUNC_ERROR();
			}

			injectedContext.ContextFlags = CONTEXT_CONTROL;
			do {
				SwitchToThread();
				if (!GetThreadContext(hRemoteThread, &injectedContext)) {
					success = FALSE;
					MSDN_FUNC_ERROR();
				}
			} while (!(injectedContext.Rip == (QWORD)remoteRip + 1));
		}
	}
	__finally {
	}
	return success;
}

BOOL writeAndAllocProcessMemory(_In_ HANDLE hRemoteProcess, _In_ HANDLE hRemoteThread, _In_ BYTE * buffer, _In_ DWORD bufferSize, _Out_ BYTE ** pRemoteBuffer) {
	BOOL success = TRUE;
	BYTE * remoteBuffer = NULL;
	CONTEXT initialContext;
	DWORD byteWritten = 0;

	__try {

		if (!takeThreadControl(hRemoteProcess, hRemoteThread, &initialContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
	
		if (!myVirtualAllocEx(hRemoteThread, &initialContext, bufferSize, &remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!myWriteProcessMemory(hRemoteThread, &initialContext, buffer, bufferSize, remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!releaseThreadControl(hRemoteThread, &initialContext)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

	}
	__finally {
		if (success) {
			*pRemoteBuffer = remoteBuffer;
		}

	}

	return success;
}

BOOL remoteGetProcAddress(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ char * arg1, _In_ char * arg2, _Outptr_ void ** pFunction) {
	BOOL success = TRUE;
	DWORD arg1Size, arg2Size;
	BYTE * remoteBuffer;
	void * hModule;

	__try {
		if (arg1 == NULL || arg2 == NULL) {
			SetLastError(ERROR_INVALID_PARAMETER);
			MSDN_FUNC_ERROR();
		}

		for (arg1Size = 0; arg1[arg1Size] != '\0'; arg1Size++);
		arg1Size++;
		for (arg2Size = 0; arg2[arg2Size] != '\0'; arg2Size++);
		arg2Size++;

		if (!myVirtualAllocEx(hRemoteThread, pInitialContext, (arg1Size < arg2Size ? arg2Size : arg1Size) + 8, &remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, arg1, arg1Size, remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!remoteCall(hRemoteThread, pInitialContext, GetModuleHandleA, remoteBuffer, NULL, NULL, NULL, &hModule)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, arg2, arg2Size, remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!remoteCall(hRemoteThread, pInitialContext, GetProcAddress, hModule, remoteBuffer, NULL, NULL, pFunction)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

	}
	__finally {

	}

	return success;
}

BOOL remoteLoadLibrary(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ const TCHAR * dllPath) {
	BOOL success = TRUE;
	DWORD dllPathSize;
	BYTE * remoteBuffer;
	void * loadLibraryAddress;
	QWORD retValue;

	__try {
		if (dllPath == NULL) {
			SetLastError(ERROR_INVALID_PARAMETER);
			MSDN_FUNC_ERROR();
		}

		for (dllPathSize = 0; dllPath[dllPathSize] != _T('\0'); dllPathSize++);
		dllPathSize++;

		if (!myVirtualAllocEx(hRemoteThread, pInitialContext, (dllPathSize*sizeof(TCHAR)) + 8, &remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, (BYTE*)dllPath, (dllPathSize * sizeof(TCHAR)), remoteBuffer)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

#ifdef UNICODE
		loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
#else
		loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
#endif

		if (!remoteCall(hRemoteThread, pInitialContext, loadLibraryAddress, remoteBuffer, NULL, NULL, NULL, (void**)&retValue)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
	}
	__finally {

	}

	return success;
}

BOOL CreateCallOnTheFly(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ void * function, _In_opt_ void * arg1, _In_opt_ void * arg2, _Outptr_ void ** page) {
	BOOL success = TRUE;
	BYTE * pageFunction;
	DWORD pageFunctionIndex = 0;

	__try {
		if (function == NULL) {
			SetLastError(ERROR_INVALID_PARAMETER);
			success = FALSE;
			MSDN_FUNC_ERROR();
		}


		if (!myVirtualAllocEx(hRemoteThread, pInitialContext, USN_PAGE_SIZE, &pageFunction)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, &START_CALL_INSIDE_THREAD, (DWORD)(&END_CALL_INSIDE_THREAD - &START_CALL_INSIDE_THREAD), pageFunction)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		pageFunctionIndex = (DWORD)(&MOV_RAX_CALL - &START_CALL_INSIDE_THREAD);
		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, (BYTE*)&function, 8, pageFunction + pageFunctionIndex)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		pageFunctionIndex = (DWORD)(&MOV_RCX_ARG1 - &START_CALL_INSIDE_THREAD);
		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, (BYTE*)&arg1, 8, pageFunction + pageFunctionIndex)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		pageFunctionIndex = (DWORD)(&MOV_RDX_ARG2 - &START_CALL_INSIDE_THREAD);
		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, (BYTE*)&arg2, 8, pageFunction + pageFunctionIndex)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		*page = pageFunction;
	}
	__finally {

	}

	return success;
}

BOOL HijackRunningThread(_In_ HANDLE hRemoteThread, _In_ CONTEXT * pInitialContext, _In_ void * function, _In_opt_ void * arg1, _In_opt_ void * arg2, _Outptr_ void ** page) {
	BOOL success = TRUE;
	OPCODE Opcode[16];
	BOOL isInitialContextSet = FALSE;
	
	__try {
		
		if (function == NULL) {
			SetLastError(ERROR_INVALID_PARAMETER);
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		
		if (!CreateCallOnTheFly(hRemoteThread, pInitialContext, function, arg1, arg2, page)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		pushimm64(Opcode, pInitialContext->Rip);
		Opcode[13] = NOP;
		Opcode[14] = NOP;
		Opcode[15] = NOP;
		pInitialContext->Rip = (DWORD64)*page;
		if (!myWriteProcessMemory(hRemoteThread, pInitialContext, Opcode, 13, *page)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		pInitialContext->ContextFlags = CONTEXT_ALL;
	}
	__finally {

	}

	return success;
}