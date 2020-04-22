#include "FlowMain.h"
extern DWORD g_TlsIdDocker;
extern MAPPED_MODULE_MANAGEMENT g_mmm;


void(*g_Exceptioncallback)(EMULATOR_HANDLER*, CONTEXT *, DWORD) = NULL;

//the function do not return if it succeed
BOOLEAN ExceptionHandlerCreatedInvalidAccess(PEXCEPTION_PARAMETER pExceptionParameter) {
	BOOLEAN success = TRUE;
	INSTRUCTION_READ instruction;
	THREAD_CONTEXT threadContext;
	MEMORY_ACCESS memAccess;
	BOOLEAN isThereMemAccess;
	DWORD i;
	DWORD oldProtect;
	HMODULE hNtdll = NULL;
	void(*NtContinue)(CONTEXT * context, BOOLEAN iretRequired) = NULL;
	DWORD lastError;
	DWORD wait = TRUE;

	__try {
		lastError = GetLastError();
		if (pExceptionParameter->exceptionRecord.ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
			success = FALSE;
			__leave;
		}

		if (!GetInstruction((OPCODE*)pExceptionParameter->context.Rip, &instruction)) {
			success = FALSE;
			__leave;
		}
		threadContext.EFlags = pExceptionParameter->context.EFlags;
		threadContext.Rip = pExceptionParameter->context.Rip;
		threadContext.Rax = pExceptionParameter->context.Rax;
		threadContext.Rcx = pExceptionParameter->context.Rcx;
		threadContext.Rdx = pExceptionParameter->context.Rdx;
		threadContext.Rbx = pExceptionParameter->context.Rbx;
		threadContext.Rsp = pExceptionParameter->context.Rsp;
		threadContext.Rbp = pExceptionParameter->context.Rbp;
		threadContext.Rsi = pExceptionParameter->context.Rsi;
		threadContext.Rdi = pExceptionParameter->context.Rdi;
		threadContext.R8 = pExceptionParameter->context.R8;
		threadContext.R9 = pExceptionParameter->context.R9;
		threadContext.R10 = pExceptionParameter->context.R10;
		threadContext.R11 = pExceptionParameter->context.R11;
		threadContext.R12 = pExceptionParameter->context.R12;
		threadContext.R13 = pExceptionParameter->context.R13;
		threadContext.R14 = pExceptionParameter->context.R14;
		threadContext.R15 = pExceptionParameter->context.R15;

		if (!GetMemoryAccessFromInstructionRead(&instruction, &threadContext, &memAccess, &isThereMemAccess)) {
			success = FALSE;
			__leave;
		}

		if (!isThereMemAccess) {
			success = FALSE;
			__leave;
		}

		do {
			wait = InterlockedBitTestAndSetAcquire(&(g_mmm.SynchronizationByte), 0);//wait and take lock
		} while (wait);



		for (i = 0; i < g_mmm.ElementLength; i++) {
			if (g_mmm.Element[i].IsModuleModifiable == TRUE && 
				g_mmm.Element[i].OldCode <= (OPCODE*)memAccess.BaseAddress + memAccess.Size / 8 &&
				g_mmm.Element[i].OldCode + g_mmm.Element[i].OldCodeLength + 16 >= (OPCODE*)memAccess.BaseAddress) {
				VirtualProtect(
					g_mmm.Element[i].OldCode,
					g_mmm.Element[i].OldCodeLength + (SIZE_T)16,
					g_mmm.Element[i].OriginalProtect,
					&oldProtect
				);
				ModuleMappingClearModule(i);
				i--;
			}
		}

		g_mmm.SynchronizationByte = 0;



		if ((hNtdll = GetModuleHandleA("ntdll")) == NULL) {
			success = FALSE;
			__leave;
		}
		if ((NtContinue = (void(*)(CONTEXT * context, BOOLEAN iretRequired))GetProcAddress(hNtdll, "NtContinue")) == NULL) {
			success = FALSE;
			__leave;
		}
	}
	__finally {
		 SetLastError(lastError);
		 if (success) {
			 NtContinue(&pExceptionParameter->context, TRUE);
		 }
		
	}
	return success;
}

void ExceptionHandlerRun() {
	PEXCEPTION_PARAMETER pExceptionParameter;
	CONTEXT * exceptionHandlerContext;
	DOCKER * docker;
	//QWORD rip;
	exceptionHandlerContext = getRegister();
	exceptionHandlerContext->Rip = (DWORD64)getCallAddress();
	
	


	pExceptionParameter = (PEXCEPTION_PARAMETER)exceptionHandlerContext->Rsp;

	ExceptionHandlerCreatedInvalidAccess(pExceptionParameter);

	docker = TlsGetValue(g_TlsIdDocker);
	if (docker == NULL) {
		return;
	}
	docker->Rip = getCallAddress();



//	rip = ((CONTEXT*)exceptionHandlerContext->Rsp)->Rip;
//	((CONTEXT*)exceptionHandlerContext->Rsp)->Rip = (DWORD64)docker->lastRip;

	//ALLOW to get rip from cache
/*	for (i = 0; i < g_mmm.ElementLength; i++) {//Find the dll currently executed
		if (g_mmm.Element[i].NewCode <= rip && rip >= g_mmm.Element[i].NewCode + g_mmm.Element[i].NewCodeLength) {
			for (j = 0; j < g_mmm.Element[i].OldCodeMaskLength; j++) {//List the instruction
				//No need to handle misaligned instruction inside the cache
				//If we find an instruction
				if (g_mmm.Element[i].OldCodeMask[j] != NO_LINKED_INSTRUCTION) {
					//If the rip match
					if (g_mmm.Element[i].OldCodeMask[j] + g_mmm.Element[i].NewCode == rip) {
						//we set the rip of the error to the corresponding instruction
						((CONTEXT*)context->Rsp)->Rip = j + g_mmm.Element[i].OldCode;
					}
				}
			}
		}
	}




	if (g_Exceptioncallback) {
		g_Exceptioncallback((EMULATOR_HANDLER*)docker, context, CALL_BACK_MISCALLENOUS_EXCEPTION);
	}*/

	saveCallAddress(docker->codeBaseAddress);
}

void ExceptionHandlerInit() {
	void * _KiUserExceptionDispatch = NULL;
	HMODULE hNtdll = GetModuleHandle(_T("ntdll"));
	MODULEINFO moduleInfo;

	if (!hNtdll) {
		return;
	}
	//Find _KiUserExceptionDispatch signature
	GetModuleInformation(GetCurrentProcess(), hNtdll, &moduleInfo, sizeof(MODULEINFO));
	for (DWORD i = 0; i < moduleInfo.SizeOfImage - 3; i++) {
		if ((*(BYTE*)((BYTE*)moduleInfo.lpBaseOfDll + i)) == 0xFC &&
			(*(DWORD*)((BYTE*)moduleInfo.lpBaseOfDll + i + 8)) == 0x74C08548) {
			_KiUserExceptionDispatch = (DWORD*)((BYTE*)moduleInfo.lpBaseOfDll + i);
			break;
		}
	}

	if (!_KiUserExceptionDispatch) {
		return;
	}
	if (!placeHookTrampoline(_KiUserExceptionDispatch, NULL, ExceptionHandlerRun, NULL)) {
		return;
	}
	return;

}