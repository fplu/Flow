#include "FlowMain.h"

extern OPCODE GetGs;

extern OPCODE START_CALLBACK;
extern OPCODE JMP_RET_CALLBACK;
extern OPCODE JMP_PTR_CALLBACK;
extern OPCODE END_CALLBACK;

extern OPCODE START_SAVE_CONTEXT;
extern OPCODE START_LOAD_CONTEXT;

extern OPCODE ADDRESS_PTR;
extern OPCODE NEW_CONTEXT_PTR;
extern OPCODE START_LOAD_COMPLETE_CONTEXT;

void(*g_ThreadInitializationCallback)(EMULATOR_HANDLER*);

DWORD g_TlsIdDocker = 0;
DWORD g_TlsIdArg = 0;
DWORD g_TlsIdStoreR1 = 0;
DWORD g_TlsIdStoreR2 = 0;
DWORD g_TlsIdStoreR3 = 0;
DWORD g_TlsIdStorePage = 0;

DOCKER_THRAD_ID g_dockerThreadId[100];//init tab
DWORD g_dockerThreadIdLength = 0;
DWORD g_dockerThreadIdSync = 0;

BYTE g_TlsAllocationMask[DOCKER_TLS_SIZE];

void(*g_ProcessInitializationCallback)(void) = NULL;
BOOLEAN isProcessInitializationDone = FALSE;
void(*g_InstructionDefaultCallback)(EMULATOR_HANDLER*) = NULL;
void(*(*g_GetInstructionCallback)(INSTRUCTION_READ*))(EMULATOR_HANDLER*) = NULL;
void(*g_ExceptionCallback)(EMULATOR_HANDLER*, CONTEXT*, DWORD) = NULL;

QWORD initIncRip(_In_ void* _firstRip, _In_opt_ void* _firstStack) {
	//__debugbreak();

	DOCKER* docker;
	BYTE* stack;
	DWORD dwBytesWritten = 0;
	DWORD dockerSize;
	DWORD traceLoopSize;
	DWORD callbackSize;
	DWORD allocationSize;
	DWORD oldProtect;
	OPCODE* code;
	BYTE* callStack;
	PPAGE_CALL_INFO pageCallInfo;
	LONG wait;
	//BOOL wait;



	if ((docker = TlsGetValue(g_TlsIdDocker)) == NULL) {



#define ceilToPage(a) (((a + USN_PAGE_SIZE - 1)/ USN_PAGE_SIZE) * USN_PAGE_SIZE)
		dockerSize = ceilToPage(sizeof(DOCKER));
		traceLoopSize = (DWORD)(USN_PAGE_SIZE + ceilToPage((ULONG_PTR)(&traceLoopEnd) - (ULONG_PTR)(&traceLoopPreStart) + BUFFER_OPCODE_SIZE));
		callbackSize = (DWORD)(USN_PAGE_SIZE + ceilToPage((ULONG_PTR)(&traceLoopEnd) - (ULONG_PTR)(&traceLoopTraceINSTRUCTION) + 13 + BUFFER_OPCODE_SIZE));
#undef ceilToPage

		allocationSize = dockerSize + traceLoopSize + callbackSize;

		if ((docker = VirtualAlloc(NULL, allocationSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL) {
			return 0;
		}
		docker->codeBaseAddress = ((BYTE*)docker) + dockerSize + USN_PAGE_SIZE;//+USN_PAGE_SIZE because first page is data
		docker->callbackBaseAddress = ((BYTE*)docker->codeBaseAddress) + traceLoopSize;//+USN_PAGE_SIZE implicit due to precendent one because first page is data

		VirtualProtect(docker, dockerSize, PAGE_READWRITE, &oldProtect);
		VirtualProtect(docker->codeBaseAddress - USN_PAGE_SIZE, USN_PAGE_SIZE, PAGE_READWRITE, &oldProtect);
		VirtualProtect(docker->callbackBaseAddress - USN_PAGE_SIZE, USN_PAGE_SIZE, PAGE_READWRITE, &oldProtect);

		docker->OFFSET_ADDRESS_PTR = (DWORD)((ULONG_PTR)&ADDRESS_PTR - (ULONG_PTR)&traceLoopPreStart);
		docker->OFFSET_NEW_CONTEXT_PTR = (DWORD)((ULONG_PTR)&NEW_CONTEXT_PTR - (ULONG_PTR)&traceLoopPreStart);
		docker->OFFSET_START_LOAD_COMPLETE_CONTEXT = (DWORD)((ULONG_PTR)&START_LOAD_COMPLETE_CONTEXT - (ULONG_PTR)&traceLoopPreStart);

		TlsSetValue(g_TlsIdDocker, docker);

		if ((stack = VirtualAlloc(NULL, USN_PAGE_SIZE * 11, MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			return 0;
		}

		docker->instructionId = -1;

		docker->fullContext.SegGs = (WORD)(docker->myGs = ((QWORD(*)(void)) & GetGs)());

		MovMyAssembly(
			&traceLoopData,
			&traceLoopPreStart,
			(DWORD)((QWORD)&traceLoopEnd - (QWORD)&traceLoopData),
			(QWORD[]) {
			(QWORD)&docker->context, (ULONG_PTR)stack + (ULONG_PTR)USN_PAGE_SIZE, (QWORD)docker, (QWORD)CallbackRun
		},
			4,
				docker->callbackBaseAddress
				);

		code = docker->callbackBaseAddress + (ULONG_PTR)&START_SAVE_CONTEXT - (ULONG_PTR)&traceLoopPreStart;
		movQwordRegister(code, &docker->context.Rax, RAX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rcx, RCX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rdx, RDX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rbx, RBX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rsp, RSP_ID); code += 7;
		movQwordRegister(code, &docker->context.Rbp, RBP_ID); code += 7;
		movQwordRegister(code, &docker->context.Rsi, RSI_ID); code += 7;
		movQwordRegister(code, &docker->context.Rdi, RDI_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R8, R8_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R9, R9_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R10, R10_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R11, R11_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R12, R12_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R13, R13_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R14, R14_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R15, R15_ID); code += 7;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm0, XMM0_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm1, XMM1_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm2, XMM2_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm3, XMM3_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm4, XMM4_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm5, XMM5_ID); code += 8;
		//	movMemEqualSeg(code, &docker->fullContext.SegGs, GS_ID); code += 6;
		//	movSegEqualMem(code, &docker->myGs, GS_ID); code += 6;

		code = docker->callbackBaseAddress + (ULONG_PTR)&START_LOAD_CONTEXT - (ULONG_PTR)&traceLoopPreStart;
		movRegisterQword(code, &docker->context.Rax, RAX_ID); code += 7;
		movRegisterQword(code, &docker->context.Rcx, RCX_ID); code += 7;
		movRegisterQword(code, &docker->context.Rdx, RDX_ID); code += 7;
		movRegisterQword(code, &docker->context.Rsp, RSP_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R8, R8_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R9, R9_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R10, R10_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R11, R11_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R15, R15_ID); code += 7;
		movdqaMemToXmm0_7(code, &docker->context.Xmm0, XMM0_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm1, XMM1_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm2, XMM2_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm3, XMM3_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm4, XMM4_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm5, XMM5_ID); code += 8;
		//	movSegEqualMem(code, &docker->fullContext.SegGs, GS_ID); code += 6;

		MovMyAssembly(
			&traceLoopData,
			&traceLoopPreStart,
			(DWORD)((QWORD)&traceLoopEnd - (QWORD)&traceLoopData),
			(QWORD[]) {
			(QWORD)&docker->context, (ULONG_PTR)stack + (ULONG_PTR)USN_PAGE_SIZE, (QWORD)docker
		},
			3,
				docker->codeBaseAddress
				);

		code = docker->codeBaseAddress + (ULONG_PTR)&START_SAVE_CONTEXT - (ULONG_PTR)&traceLoopPreStart;
		movQwordRegister(code, &docker->context.Rax, RAX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rcx, RCX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rdx, RDX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rbx, RBX_ID); code += 7;
		movQwordRegister(code, &docker->context.Rsp, RSP_ID); code += 7;
		movQwordRegister(code, &docker->context.Rbp, RBP_ID); code += 7;
		movQwordRegister(code, &docker->context.Rsi, RSI_ID); code += 7;
		movQwordRegister(code, &docker->context.Rdi, RDI_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R8, R8_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R9, R9_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R10, R10_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R11, R11_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R12, R12_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R13, R13_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R14, R14_ID); code += 7;
		movQwordRegisterR8_R15(code, &docker->context.R15, R15_ID); code += 7;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm0, XMM0_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm1, XMM1_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm2, XMM2_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm3, XMM3_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm4, XMM4_ID); code += 8;
		movdqaXmm0_7ToMem(code, &docker->context.Xmm5, XMM5_ID); code += 8;
		//	movMemEqualSeg(code, &docker->fullContext.SegGs, GS_ID); code += 6;
		//	movSegEqualMem(code, &docker->myGs, GS_ID); code += 6;

		code = docker->codeBaseAddress + (ULONG_PTR)&START_LOAD_CONTEXT - (ULONG_PTR)&traceLoopPreStart;
		movRegisterQword(code, &docker->context.Rax, RAX_ID); code += 7;
		movRegisterQword(code, &docker->context.Rcx, RCX_ID); code += 7;
		movRegisterQword(code, &docker->context.Rdx, RDX_ID); code += 7;
		movRegisterQword(code, &docker->context.Rsp, RSP_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R8, R8_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R9, R9_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R10, R10_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R11, R11_ID); code += 7;
		movRegisterQwordR8_R15(code, &docker->context.R15, R15_ID); code += 7;
		movdqaMemToXmm0_7(code, &docker->context.Xmm0, XMM0_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm1, XMM1_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm2, XMM2_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm3, XMM3_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm4, XMM4_ID); code += 8;
		movdqaMemToXmm0_7(code, &docker->context.Xmm5, XMM5_ID); code += 8;
		//	movSegEqualMem(code, &docker->fullContext.SegGs, GS_ID); code += 6;

#define relativeAddress(address_) ((ULONG_PTR)docker->codeBaseAddress + (ULONG_PTR)address_ - (ULONG_PTR)&traceLoopPreStart)
		docker->traceLoopStart = (OPCODE*)relativeAddress(&traceLoopStart);
		docker->traceLoopTraceInstruction = (OPCODE*)relativeAddress(&traceLoopTraceINSTRUCTION);
		docker->traceLoopEnd = (OPCODE*)relativeAddress(&traceLoopEnd);
#undef relativeAddress

		//		docker->isStepEnable = TRUE;
//		docker->enableDefaultStepCallback = TRUE;

	//	docker->breakPointLength = 0;
		//docker->callCallbackLength = 0;
		//docker->retBreakpointLength = 0;
		//docker->retCallbackLength = 0;

		docker->syscallPostCallback = NULL;

		docker->Flag = 0;
		docker->SuspendCount = 0;

		callStack = VirtualAlloc(NULL, USN_PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		pageCallInfo = VirtualAlloc(NULL, PAGE_CALL_INFO_SIZE * sizeof(PAGE_CALL_INFO), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		*(QWORD*)(callStack + USN_PAGE_SIZE - 0x10) = (QWORD)pageCallInfo;
		docker->pageCallInfo = pageCallInfo;
		TlsSetValue(g_TlsIdStorePage, callStack);
		//TlsSetValue(g_TlsIdStoreCallStackLength, 0);

		/*if(dockerPageLength < DOCKER_TLS_SIZE - 1) {
			BOOL wait;
			do {
				wait = InterlockedBitTestAndSetAcquire(&dockerPageLock, 0);
			} while (wait);

			dockerPage[dockerPageLength].base = docker;
			dockerPage[dockerPageLength].size = allocationSize;
			dockerPageLength++;

			dockerPageLock = 0;
		}*/
		docker->Rip = _firstRip;

		docker->ptitCountPtetPasTemp = 0;
		docker->ptitCountPtetTemp = 0;


		do {
			wait = InterlockedBitTestAndSetAcquire(&(g_dockerThreadIdSync), 0);//wait and take lock
		} while (wait);

		g_dockerThreadId[g_dockerThreadIdLength].Docker = docker;
		g_dockerThreadId[g_dockerThreadIdLength].Sync = 0;
		g_dockerThreadId[g_dockerThreadIdLength].ThreadId = GetCurrentThreadId();
		g_dockerThreadIdLength++;

		InterlockedBitTestAndResetRelease(&(g_dockerThreadIdSync), 0);
	}

	docker->Rip = _firstRip;



	if (g_ThreadInitializationCallback) {
		g_ThreadInitializationCallback((EMULATOR_HANDLER*)docker);
	}

	//__debugbreak();
	return ((QWORD(*)(void*))docker->codeBaseAddress)(_firstStack);

}