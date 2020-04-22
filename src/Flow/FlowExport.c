#include "FlowMain.h"

TCHAR * g_AllDependencies[10] = { NULL };
DWORD g_AllDependenciesLength = 0;

extern void(*g_ProcessInitializationCallback)(void);
extern void(*g_ThreadInitializationCallback)(EMULATOR_HANDLER*);
extern void(*g_InstructionDefaultCallback)(EMULATOR_HANDLER*);
extern void(*(*g_GetInstructionCallback)(INSTRUCTION_READ*))(EMULATOR_HANDLER*);
//extern void(*g_SyscallDefaultCallback)(EMULATOR_HANDLER*, INSTRUCTION_READ*);

//extern void(*g_DefaultCallback)(EMULATOR_HANDLER *, DWORD);
extern void(*g_ExceptionCallback)(EMULATOR_HANDLER*, CONTEXT *, DWORD);
HANDLE g_UserModule = NULL;

//extern SYSCALL_CALLBACK g_SyscallCallback[0x1000];
//extern DWORD g_SyscallCallbackLength;
//to put inside the dll main to hijack newly created thread
void debugCurrentThread(void) {
	if (TlsGetValue(g_TlsIdDocker) == 0) {
		((void(*)())TAKE_THIS())();
	}
}

MAPPED_MODULE_PROCESSING getMappingState(void) {
	return g_mmp;
}

/*
	First function to be called during the injection
	This function is follow by a TAKE_THIS
	See /injection.(c|h|(asm))/i files to understand why we can get the string this way and without size check for both modulesNames and numberOfModule
	WARNING SOME ASSEMBLY STUFF DEPEND ON THIS NAME
*/
void globalFlowInit(_In_ WCHAR * modulesNames, _In_ DWORD numberOfModule) {
	DWORD i;
	void * tmp;
	//void(*dllInitialization)(void);
	if (numberOfModule <= 0) {
		return;
	}
	 
	g_AllDependenciesLength = numberOfModule;
	for (i = 0; i < g_AllDependenciesLength; i++) {
		g_AllDependencies[i] = modulesNames + ((size_t)500 * i);
	}

	g_UserModule = GetModuleHandleW(g_AllDependencies[numberOfModule - 1]);
	//g_DefaultCallback = (void(*)(EMULATOR_HANDLER*, DWORD))GetProcAddress(g_UserModule, "defaultCallback");
	g_ExceptionCallback = (void(*)(EMULATOR_HANDLER*, CONTEXT*, DWORD))GetProcAddress(g_UserModule, "exceptionCallback");
	g_ProcessInitializationCallback = (void(*)(void))GetProcAddress(g_UserModule, "processInitializationCallback");
	g_ThreadInitializationCallback = (void(*)(EMULATOR_HANDLER*))GetProcAddress(g_UserModule, "threadInitializationCallback");
	g_InstructionDefaultCallback = (void(*)(EMULATOR_HANDLER*))GetProcAddress(g_UserModule, "instructionDefaultCallback");
	g_GetInstructionCallback = (void(*(*)(INSTRUCTION_READ*))(EMULATOR_HANDLER*))GetProcAddress(g_UserModule, "getInstructionCallback");
	g_SyscallDefaultCallback = (syscallCallback)GetProcAddress(g_UserModule, "syscallDefaultCallback");
	
	tmp = (void*)GetProcAddress(g_UserModule, "enableRangeInstructionAnalyse");
	g_EnableRangeInstructionAnalyseFunction = (tmp == NULL ? g_EnableRangeInstructionAnalyseFunction : (BOOL(*)(OPCODE * baseAddress, DWORD baseSize))tmp);

	if (g_ProcessInitializationCallback) {
		g_ProcessInitializationCallback();
	}
	/*dllInitialization = (void(*)(void))GetProcAddress(g_UserModule, "dllInitialization");
	if (dllInitialization != NULL) {
		dllInitialization();
	}*/
}

/*void enableStepOut(_Inout_ EMULATOR_HANDLER * pEmulatorHandler) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	docker->stepCounter = 1;
}

void enableStepOver(_Inout_ EMULATOR_HANDLER * pEmulatorHandler) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	docker->stepCounter = 0;
}

void enableStepIn(_Inout_ EMULATOR_HANDLER * pEmulatorHandler) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	docker->isStepEnable = TRUE;
}*/

/*void closeThread(void) {
	DWORD exitCode;
	GetExitCodeThread(GetCurrentThread(), &exitCode);
	TerminateThread(GetCurrentThread(), exitCode);
}*/

BOOL addCallCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ void * function, _In_ void(callback_)(EMULATOR_HANDLER* docker, QWORD lastRip) ) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;

	__debugbreak();
	return TRUE;
}

BOOL addRetCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ void * function, _In_ void(callback_)(EMULATOR_HANDLER* docker, QWORD lastRip)) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;

	__debugbreak();
	return TRUE;
}

/*
	do not set syscallId to -1
*/
void addSyscallCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ syscallCallback _syscallCallback, _In_ DWORD syscallId) {
	if (syscallId == -1 || _syscallCallback == NULL) {
		return;
	}
	g_SyscallCallback[g_SyscallCallbackLength].address = _syscallCallback;
	g_SyscallCallback[g_SyscallCallbackLength].id = syscallId;
	g_SyscallCallbackLength++;
}

/*
	_syscallCallback = NULL ignored
	syscallId = -1 ignored
*/
void removeSyscallCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_opt_ DWORD syscallId, _In_opt_ syscallCallback _syscallCallback) {
	DWORD i;
	for (i = 0; i < g_SyscallCallbackLength; i++) {
		if ((syscallId == -1 || g_SyscallCallback[i].id == syscallId) &&
			(_syscallCallback == NULL || g_SyscallCallback[i].address == _syscallCallback)) {
			g_SyscallCallbackLength--;
			g_SyscallCallback[i].address = g_SyscallCallback[g_SyscallCallbackLength].address;
			g_SyscallCallback[i].id = g_SyscallCallback[g_SyscallCallbackLength].id;
			i--;
		}
	}
}

void getThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _Out_ THREAD_CONTEXT * threadContext) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	*threadContext = docker->context;
	threadContext->Rip = (DWORD64)docker->Rip;
}

/*
	only set the callee-safe register
*/
void setThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ THREAD_CONTEXT * threadContext) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	THREAD_CONTEXT tmpContext;

	tmpContext = *threadContext;
	tmpContext.Rsp = docker->context.Rsp;
	tmpContext.Rbp = docker->context.Rbp;
	tmpContext.Rsi = docker->context.Rsi;
	tmpContext.Rdi = docker->context.Rdi;
	tmpContext.R12 = docker->context.R12;
	tmpContext.R13 = docker->context.R13;
	tmpContext.R14 = docker->context.R14;
	tmpContext.R15 = docker->context.R15;

	*threadContext = tmpContext;
	docker->context = tmpContext;
	docker->Rip = (void*)threadContext->Rip;

}

/*
	when use MUST :
		- be called first by the Callback function
		- the Callback function must not modify segment register
		- the Callback function must not use floating operation or other xmm, ymm and zmm operation
		- the Callback function could call function that use floating operation or other xmm, ymm and zmm operation
*/
void getExtendedThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _Out_ CONTEXT * threadContext) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	CONTEXT tmpContext;
	_getAllRegister(&tmpContext);
	tmpContext.Rax = docker->context.Rax;
	tmpContext.Rbx = docker->context.Rbx;
	tmpContext.Rcx = docker->context.Rcx;
	tmpContext.Rdx = docker->context.Rdx;
	tmpContext.Rsp = docker->context.Rsp;
	tmpContext.Rbp = docker->context.Rbp;
	tmpContext.Rsi = docker->context.Rsi;
	tmpContext.Rdi = docker->context.Rdi;
	tmpContext.R8 = docker->context.R8;
	tmpContext.R9 = docker->context.R9;
	tmpContext.R10 = docker->context.R10;
	tmpContext.R11 = docker->context.R11;
	tmpContext.R12 = docker->context.R12;
	tmpContext.R13 = docker->context.R13;
	tmpContext.R14 = docker->context.R14;
	tmpContext.R15 = docker->context.R15;
	tmpContext.Xmm0 = docker->context.Xmm0;
	tmpContext.Xmm1 = docker->context.Xmm1;
	tmpContext.Xmm2 = docker->context.Xmm2;
	tmpContext.Xmm3 = docker->context.Xmm3;
	tmpContext.Xmm4 = docker->context.Xmm4;
	tmpContext.Xmm5 = docker->context.Xmm5;
	tmpContext.EFlags = (DWORD)docker->context.EFlags;
	tmpContext.Rip = docker->context.Rip;

	*threadContext = tmpContext;
	threadContext->Rip = (DWORD64)docker->Rip;
}

/*
	Set xmm, segment, the INSTRUCTION pointer (Rip) and Rax to R15 registers except cs segment
*/
void setExtendedThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ CONTEXT * threadContext) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	THREAD_CONTEXT tmpContext;
	tmpContext.Rax = threadContext->Rax;
	tmpContext.Rbx = threadContext->Rbx;
	tmpContext.Rcx = threadContext->Rcx;
	tmpContext.Rdx = threadContext->Rdx;
	tmpContext.Rsp = threadContext->Rsp;
	tmpContext.Rbp = threadContext->Rbp;
	tmpContext.Rsi = threadContext->Rsi;
	tmpContext.Rdi = threadContext->Rdi;
	tmpContext.R8 = threadContext->R8;
	tmpContext.R9 = threadContext->R9;
	tmpContext.R10 = threadContext->R10;
	tmpContext.R11 = threadContext->R11;
	tmpContext.R12 = threadContext->R12;
	tmpContext.R13 = threadContext->R13;
	tmpContext.R14 = threadContext->R14;
	tmpContext.R15 = threadContext->R15;
	tmpContext.Xmm0 = threadContext->Xmm0;
	tmpContext.Xmm1 = threadContext->Xmm1;
	tmpContext.Xmm2 = threadContext->Xmm2;
	tmpContext.Xmm3 = threadContext->Xmm3;
	tmpContext.Xmm4 = threadContext->Xmm4;
	tmpContext.Xmm5 = threadContext->Xmm5;
	tmpContext.EFlags = threadContext->EFlags;
	tmpContext.Rip = threadContext->Rip;

	docker->context = tmpContext;
	docker->fullContext = *threadContext;
	docker->Rip = (void*)threadContext->Rip;

	*(QWORD*)(docker->callbackBaseAddress + docker->OFFSET_ADDRESS_PTR) = (QWORD)(docker->callbackBaseAddress + docker->OFFSET_START_LOAD_COMPLETE_CONTEXT);
	*(QWORD*)(docker->callbackBaseAddress + docker->OFFSET_NEW_CONTEXT_PTR) = (QWORD)(&docker->fullContext);
	*(QWORD*)(docker->codeBaseAddress + docker->OFFSET_ADDRESS_PTR) = (QWORD)(docker->codeBaseAddress + docker->OFFSET_START_LOAD_COMPLETE_CONTEXT);
	*(QWORD*)(docker->codeBaseAddress + docker->OFFSET_NEW_CONTEXT_PTR) = (QWORD)(&docker->fullContext);
}

/*void flushCache(_Inout_ EMULATOR_HANDLER * pEmulatorHandler) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	DWORD i;
	MEMORY_BASIC_INFORMATION clearMemBasicInfo = { 0 };

	for (i = 0; i < ACCESS_ADDRESS_SIZE; i++) {
		docker->accessAddress[i] = NULL;
	}
	docker->memBasicInfo = clearMemBasicInfo;
}*/

/*void setThreadDefaultCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ void(*threadDefaultCallback)(EMULATOR_HANDLER *)) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
//	docker->stepCallback = (void (__cdecl *)(DOCKER*))threadDefaultCallback;
}*/

/*
use this instead of windows tls. You have up to 256 slot.
tlsId == -1 mean fail
be carefull, there is no thread synchronization mechanism
*/
void tlsAlloc(_Out_ DWORD * pTlsId) {
	DWORD i;
	for (i = 0; i < DOCKER_TLS_SIZE; i++) {
		if (g_TlsAllocationMask[i] == 0) {
			*pTlsId = i;
			g_TlsAllocationMask[i] = 1;
			return;
		}
	}
	*pTlsId = -1;
}

/*
use this instead of windows tls. You have up to 256 slot.
*/
void tlsFree(_In_ DWORD tlsId) {
	g_TlsAllocationMask[tlsId] = 0;
}

/*
use this instead of windows tls. You have up to 256 slot.
*/
void tlsSetValue(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ DWORD tlsId, _In_ QWORD value) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	docker->tls[tlsId] = value;
}

/*
use this instead of windows tls. You have up to 256 slot.
*/
void tlsGetValue(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ DWORD tlsId, _Out_ QWORD * pValue) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	*pValue = docker->tls[tlsId];
}

/*
you MUST call this function only inside a syscall handler. A post call back for the syscall will happen ONCE with the desired function
it fail if a call back was already set
*/
BOOL setSyscallPostCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_opt_ void(*postCallback)(EMULATOR_HANDLER*)) {
	DOCKER * docker = (DOCKER*)pEmulatorHandler;
	if (docker->syscallPostCallback) {
		return FALSE;
	}
	
	docker->syscallPostCallback = (void(*)(DOCKER*))postCallback;
//	docker->isStepEnable = TRUE;
	return TRUE;
}