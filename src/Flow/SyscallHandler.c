#include "FlowMain.h"

extern TCHAR * g_AllDependencies[10];
extern DWORD g_AllDependenciesLength;

extern void(*g_ExceptionCallback)(EMULATOR_HANDLER*, CONTEXT *, DWORD);
extern BOOL g_AnalyseChild = TRUE;

DWORD SyscallId_NtContinue;
DWORD SyscallId_NtProtectVirtualMemory;
DWORD SyscallId_NtAllocateVirtualMemory;
DWORD SyscallId_NtCreateUserProcess;
DWORD SyscallId_NtResumeThread;
DWORD SyscallId_NtSuspendThread;

BYTE* ntdllKiUserExceptionDispatch;
SYSTEM_INFO systemInfo;

#define GET_CURRENT_PAGE_BASE(ptr) \
	((QWORD)ptr & (~(QWORD)(systemInfo.dwPageSize - 1)))


SYSCALL_CALLBACK g_SyscallCallback[g_SyscallCallbackMaxLength] = { 0 };
DWORD g_SyscallCallbackLength = 0;
syscallCallback g_SyscallDefaultCallback = NULL;

void SyscallHandlerInit(void) {
	//__debugbreak();
	HMODULE hNtdll = GetModuleHandleA("ntdll");
	if (hNtdll == NULL) {
		return;
	}

	SyscallId_NtContinue = ((DWORD*)(GetProcAddress(hNtdll, "NtContinue")))[1];
	SyscallId_NtProtectVirtualMemory = ((DWORD*)(GetProcAddress(hNtdll, "NtProtectVirtualMemory")))[1];
	SyscallId_NtAllocateVirtualMemory = ((DWORD*)(GetProcAddress(hNtdll, "NtAllocateVirtualMemory")))[1];
	SyscallId_NtCreateUserProcess = ((DWORD*)(GetProcAddress(hNtdll, "NtCreateUserProcess")))[1];
	SyscallId_NtResumeThread = ((DWORD*)(GetProcAddress(hNtdll, "NtResumeThread")))[1];
	SyscallId_NtSuspendThread = ((DWORD*)(GetProcAddress(hNtdll, "NtSuspendThread")))[1];


//	g_SyscallWhichSetRip[g_SyscallWhichSetRipLength] = SyscallId_NtContinue;
	GetSystemInfo(&systemInfo);

	ntdllKiUserExceptionDispatch = (BYTE*)GET_CURRENT_PAGE_BASE(GetProcAddress(hNtdll, "KiUserExceptionDispatch"));

	g_SyscallCallback[g_SyscallCallbackLength].id = SyscallId_NtContinue;
	g_SyscallCallback[g_SyscallCallbackLength].address = (syscallCallback) callbackNtContinue;
	g_SyscallCallbackLength++;

	/*g_SyscallCallback[g_SyscallCallbackLength].id = SyscallId_NtAllocateVirtualMemory;
	g_SyscallCallback[g_SyscallCallbackLength].address = (syscallCallback) callbackNtAllocateVirtualMemory;
	g_SyscallCallbackLength++;*/
	g_SyscallCallback[g_SyscallCallbackLength].id = SyscallId_NtCreateUserProcess;
	g_SyscallCallback[g_SyscallCallbackLength].address = (syscallCallback) callbackNtCreateUserProcess;
	g_SyscallCallbackLength++;
/**/
	g_SyscallCallback[g_SyscallCallbackLength].id = SyscallId_NtProtectVirtualMemory;
	g_SyscallCallback[g_SyscallCallbackLength].address = (syscallCallback)callbackNtProtectVirtualMemory;
	g_SyscallCallbackLength++;
/**/
/*/
	g_SyscallCallback[g_SyscallCallbackLength].id = SyscallId_NtResumeThread;
	g_SyscallCallback[g_SyscallCallbackLength].address = (syscallCallback)callbackNtResumeThread;
	g_SyscallCallbackLength++;
/**/
/*/
	g_SyscallCallback[g_SyscallCallbackLength].id = SyscallId_NtSuspendThread;
	g_SyscallCallback[g_SyscallCallbackLength].address = (syscallCallback) callbackNtSuspendThread;
	g_SyscallCallbackLength++;
/**/

}

BOOL callbackNtContinue(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	QWORD nextRip_ = (QWORD)NULL;
	DOCKER * docker = (DOCKER*)pEmulatorHandler;

	__try {


		*rip = ((CONTEXT*)context->R10)->Rip - 2;//-2 because there will be a + 2 later due to the size of the syscall
		((CONTEXT*)context->R10)->Rip = (DWORD64)docker->traceLoopStart;

		CALLBACK_WRITE_SYSCALL();
	}
	__finally {

	}

	return success;
}


BOOL callbackNtProtectVirtualMemory(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	QWORD nextRip_ = (QWORD)NULL;
	DOCKER * docker = (DOCKER*)pEmulatorHandler;

	//MAPPED_PAGE pmp;
	QWORD startAddress;
	QWORD endAddress;
	DWORD flag;
	DWORD i;
	DWORD wait;
	BOOL wasted;
	//DWORD elementLength;
	DWORD oldProtect;
	DWORD error;

	__try {

		error = GetLastError();

		startAddress = GET_CURRENT_PAGE_BASE(*((void**)context->Rsp + 12));
		endAddress = startAddress + *((QWORD*)context->Rsp + 11);

	//	size = *(QWORD*)(context->R8);
		flag = (DWORD)(context->R9);
		

		if (!VirtualProtect((LPVOID)startAddress, *((QWORD*)context->Rsp + 11), flag, &oldProtect)) {
			CALLBACK_WRITE_SYSCALL();
			__leave;
		}

		VirtualProtect((LPVOID)startAddress, *((QWORD*)context->Rsp + 11), oldProtect, &oldProtect);
		
		for (i = 0; i < g_mmm.ElementLength; i++) {
			wasted = FALSE;
			if ((QWORD)startAddress <= (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength + (QWORD)16 &&
				(QWORD)endAddress >= (QWORD)g_mmm.Element[i].OldCode) {
				if (flag != g_mmm.Element[i].OriginalProtect) {
					wasted = TRUE;
				}
//				if (flag & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_NOACCESS | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_READONLY)) {
				//	__debugbreak();
//					wasted = TRUE;
//				}
			}
			/*for (j = 0; j < g_mmm.Element[i].OldDatas.Length; j++) {
				if (startAddress <= g_mmm.Element[i].OldDatas.Pages[j].BaseAddress &&
					endAddress >= g_mmm.Element[i].OldDatas.Pages[j].BaseAddress ||
					startAddress <= g_mmm.Element[i].OldDatas.Pages[j].BaseAddress + g_mmm.Element[i].OldDatas.Pages[j].RegionSize &&
					endAddress >= g_mmm.Element[i].OldDatas.Pages[j].BaseAddress + g_mmm.Element[i].OldDatas.Pages[j].RegionSize) {
					if (flag & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY | PAGE_NOACCESS | PAGE_READWRITE | PAGE_WRITECOPY)) {
						//__debugbreak();
						g_mmm.AreAllModuleInit = FALSE;
						wasted = TRUE;
					}
				}
			}*/

			if (wasted) {
				do {
					if (docker->Flag & DOCKER_ASK_SUSPENDED) {
						success = FALSE;
						__leave;
					}
					wait = InterlockedBitTestAndSetAcquire(&(g_mmm.SynchronizationByte), 0);//wait and take lock
				} while (wait);


				ModuleMappingClearModule(i);
				g_mmm.SynchronizationByte = 0;//reset lock

				i--;
				/*do {
					if (docker->Flag & DOCKER_ASK_SUSPENDED) {
						success = FALSE;
						__leave;
					}
					wait = InterlockedBitTestAndSetAcquire(&(g_mmm.SynchronizationByte), 0);//wait and take lock
				} while (wait);

				for (j = 0; j < g_dockerThreadIdLength; j++) {
					if (g_dockerThreadId[j].ThreadId != GetCurrentThreadId()) {
						if (!FlowSuspendThread(g_dockerThreadId[j].ThreadId)) {
							//if (GetLastError() != 6) {
							//	__debugbreak();
							//}
						}
					}
				}
				//__debugbreak();

				VirtualFree(g_mmm.Element[i].CallInfo, 0, MEM_RELEASE);
				VirtualFree(g_mmm.Element[i].Instructions, 0, MEM_RELEASE);
				VirtualFree(g_mmm.Element[i].InstructionsMinimal, 0, MEM_RELEASE);
				VirtualFree(g_mmm.Element[i].NewCode, 0, MEM_RELEASE);
				VirtualFree(g_mmm.Element[i].NewData, 0, MEM_RELEASE);
				VirtualFree(g_mmm.Element[i].OldCodeMask, 0, MEM_RELEASE);
				if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {//we clear g_ppmp
					for (j = g_mmm.Element[i].OldCode; j < g_mmm.Element[i].OldCode + g_mmm.Element[i].OldCodeLength; j++) {
						g_ppmp(j) = NULL;
					}
				}
				g_mmm.Element[i].OldCode = 0;
				g_mmm.Element[i].OldCodeLength = 0;
				g_mmm.Element[i] = g_mmm.Element[g_mmm.ElementLength - 1];
				if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {//we change g_ppmp
					for (j = g_mmm.Element[i].OldCode; j < g_mmm.Element[i].OldCode + g_mmm.Element[i].OldCodeLength; j++) {
						g_ppmp(j) = g_mmm.Element + i;
					}
				}
				g_mmm.ElementLength--;
				i--;

				for (j = 0; j < g_dockerThreadIdLength; j++) {
					if (g_dockerThreadId[j].ThreadId != GetCurrentThreadId()) {
						if (!FlowResumeThread(g_dockerThreadId[j].ThreadId)) {
							//if (GetLastError() != 6) {
							//	__debugbreak();
							//}
						}
					}
				}

				g_mmm.SynchronizationByte = 0;
				i--;
				*/
			}
		}

		if (flag & (PAGE_EXECUTE_READ | PAGE_EXECUTE)) {
	//		g_mmm.AreAllModuleInit = FALSE;
		}

		/*if (address <= ntdllKiUserExceptionDispatch &&
			address + systemInfo.dwPageSize >= ntdllKiUserExceptionDispatch) {


			if (g_ExceptionCallback != NULL) {
				g_ExceptionCallback(docker, NULL, CALL_BACK_ACCESS_RIGHT_MODIFICATION);
			}
		}*/
		//__debugbreak();
	//	__debugbreak();
		CALLBACK_WRITE_SYSCALL();

	}
	__finally {
		SetLastError(error);
	}
	return success;
}

/*BOOL callbackNtAllocateVirtualMemory(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	QWORD nextRip_ = (QWORD)NULL;
	DOCKER * docker = (DOCKER*)pEmulatorHandler;

	BYTE * address;
	QWORD size;
	DWORD flag;

	__try {
		address = (BYTE*)GET_CURRENT_PAGE_BASE(*(BYTE**)(context->Rdx));
		size = *(QWORD*)(context->R9);
		flag = *(DWORD*)(context->Rsp + 0x30);

		if (address <= ntdllKiUserExceptionDispatch &&
			address + systemInfo.dwPageSize >= ntdllKiUserExceptionDispatch) {
			if (g_ExceptionCallback != NULL) {
				g_ExceptionCallback((EMULATOR_HANDLER*)docker, NULL, CALL_BACK_ACCESS_RIGHT_MODIFICATION);
			}
		}
		CALLBACK_WRITE_SYSCALL();
	}
	__finally {

	}
	return success;
}
*/
//WCHAR * g_PathNtCreateUserProcess;
//WCHAR * g_CommandLineNtCreateUserProcess;
__declspec(thread) PHANDLE g_phProcessNtCreateUserProcess = NULL;
__declspec(thread) PHANDLE g_phThreadNtCreateUserProcess = NULL;
__declspec(thread) HANDLE g_hProcessNtCreateUserProcess = INVALID_HANDLE_VALUE;
__declspec(thread) HANDLE g_hThreadNtCreateUserProcess = INVALID_HANDLE_VALUE;
//__declspec(thread) BOOL g_CreateSuspendedNtCreateUserProcess = TRUE;
__declspec(thread) BOOL g_IsNtCreateUserProcessCurrentlyPerforming = FALSE;


BOOL callbackNtCreateUserProcess(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	QWORD nextRip_ = (QWORD)NULL;
	//DWORD commandLineLength;
	//WCHAR * commandLine;

	__try {

//		__debugbreak();
		if (!g_AnalyseChild) {
			__leave;
		}

//		path = *((void**)*((void**)context->Rsp + 9) + 0xd);
//		for (pathLength = 0; path[pathLength] != L'\0'; pathLength++);
//		g_CommandLineNtCreateUserProcess = commandLine = *((void**)*((void**)context->Rsp + 9) + 0xf);
//		for (commandLineLength = 0; path[commandLineLength] != L'\0'; commandLineLength++);
//		g_CreateSuspendedNtCreateUserProcess = (*((QWORD*)context->Rsp + 7) & 0x0000000000000200) ? TRUE : FALSE;
		//*((QWORD*)context->Rsp + 7) |= 0x0000000000000200;//CREATE_SUSPENDED

		g_phProcessNtCreateUserProcess = (PHANDLE)context->Rcx;
		g_phThreadNtCreateUserProcess = (PHANDLE)context->Rdx;
		setSyscallPostCallback(pEmulatorHandler, postCallbackNtCreateUserProcess);

		g_IsNtCreateUserProcessCurrentlyPerforming = TRUE;
		CALLBACK_WRITE_SYSCALL();
	}
	__finally {

	}
	return success;
}

BOOL postCallbackNtCreateUserProcess(_Inout_ EMULATOR_HANDLER * pEmulatorHandler) {
	BOOL success = TRUE;
//	DWORD lastError;
//	DWORD aProcesses[1024], cbNeeded, cProcesses;
//	unsigned int i;

	__try {
	
//		__debugbreak();
//		lastError = GetLastError();
		g_hProcessNtCreateUserProcess = *g_phProcessNtCreateUserProcess;
		g_hThreadNtCreateUserProcess = *g_phThreadNtCreateUserProcess;

		//if (g_CreateSuspendedNtCreateUserProcess) {//inc suspend thread count if created suspended
	//		SuspendThread(*g_hThreadNtCreateUserProcess);
		//}
		//		g_hProcessNtCreateUserProcess;

	}
	__finally {

	//	SetLastError(lastError);
	}

	return success;
}


BOOL callbackNtResumeThreadForCreateProcess(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	DWORD lastError;
	__try {

		if (g_IsNtCreateUserProcessCurrentlyPerforming && (HANDLE)context->Rcx == g_hThreadNtCreateUserProcess) {
			lastError = GetLastError();
			g_IsNtCreateUserProcessCurrentlyPerforming = FALSE;
			SuspendThread(g_hThreadNtCreateUserProcess);
			executeInjectorFunction2(g_hProcessNtCreateUserProcess, g_hThreadNtCreateUserProcess, g_AllDependencies, g_AllDependenciesLength);
			SetLastError(lastError);
		}

//		CALLBACK_WRITE_SYSCALL();
	}
	__finally {

	}

	return success;
}


BOOL callbackNtSuspendThread(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	DWORD threadId;
	//LONG wait;
	DWORD i;

	__try {
		if ((threadId = GetThreadId((HANDLE)(context->R10))) == 0) {
			CALLBACK_WRITE_SYSCALL();
			__leave;
		}

		if (GetCurrentThreadId() == threadId) {
			if (14 + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
				success = FALSE;
				__leave;
			}
			jmpImm64(buffer, *rip);
			bufferIndex += 14;
		}
		for (i = 0; i < g_dockerThreadIdLength; i++) {
			if (threadId == g_dockerThreadId[i].ThreadId) {
				break;
			}
			if (i + 1 == g_dockerThreadIdLength) {
				CALLBACK_WRITE_SYSCALL();
				__leave;
			}
		}

		if (!FlowSuspendThread(threadId)) {
			if (GetLastError() != 6) {
				__debugbreak();
			}
		}
	//	CALLBACK_WRITE_SYSCALL();
	}
	__finally {

	}
	return success;
}

BOOL callbackNtResumeThread(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex) {
	BOOL success = TRUE;
	DWORD i;
	DWORD threadId;
	__try {
		callbackNtResumeThreadForCreateProcess(context, rip, pEmulatorHandler, buffer, bufferIndex);

		if ((threadId = GetThreadId((HANDLE)(context->R10))) == 0) {
			CALLBACK_WRITE_SYSCALL();
			__leave;
		}

		for (i = 0; i < g_dockerThreadIdLength; i++) {
			if (threadId == g_dockerThreadId[i].ThreadId) {
				break;
			}
			if (i + 1 == g_dockerThreadIdLength) {
				CALLBACK_WRITE_SYSCALL();
				__leave;
			}

		}


		if (!FlowResumeThread(threadId)) {
			if (GetLastError() != 6) {
				__debugbreak();
			}
		}
//		CALLBACK_WRITE_SYSCALL();
	}
	__finally {

	}
	return success;
}
