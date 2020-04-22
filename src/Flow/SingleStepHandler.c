#include "FlowMain.h"



extern void(*g_ProcessInitializationCallback)(void);
extern BOOLEAN isProcessInitializationDone;
extern void(*g_InstructionDefaultCallback)(EMULATOR_HANDLER*);
extern void(*(*g_GetInstructionCallback)(INSTRUCTION_READ*))(EMULATOR_HANDLER*);
extern void(*g_ExceptionCallback)(EMULATOR_HANDLER*, CONTEXT*, DWORD);


void SingleStepHandlerRun(DOCKER * docker) {
	BOOL success = TRUE;
	INSTRUCTION_READ instructionRead;
	THREAD_CONTEXT * context = NULL;
	OPCODE * buffer;
	DWORD bufferIndex = 0;
	DWORD bufferLength = BUFFER_OPCODE_SIZE;
	DWORD memoryCheckValue;
	QWORD dest;
	void(*customCallback)(EMULATOR_HANDLER *) = NULL;
	//__declspec(thread) static int ptitCountPtetTemp = 0;
//	__declspec(thread) static int ptitCountPtetPasTemp = 0;

	__try {

		//Syscall post - callback
		if (docker->syscallPostCallback) {
			docker->syscallPostCallback(docker);
			docker->syscallPostCallback = NULL;
		}



		ModuleMappingMapAllUnmappedModule();
		//jmp to module mapping
		if (docker->ptitCountPtetPasTemp > 1000) {
			ModuleMappingMapPage(docker->Rip, 0x100000);
			docker->ptitCountPtetPasTemp = 0;
		}

		if (docker->instructionId != -1) {
			docker->Rip = (void*)docker->mappedInstructionMinimal[docker->instructionId].OldAddress;
			docker->instructionId = -1;
		}
		else if ((dest = ModuleMappingGetNewAddressFromOldAddress(&docker->Flag, (QWORD)docker->Rip, TRUE)) != (QWORD)NULL) {
			jmpImm64(docker->traceLoopTraceInstruction, dest);
			jmpImm64(docker->traceLoopTraceInstruction + 20, docker->traceLoopStart);//WARNING first jmpImm64 + 20 depend on some assembly code
			docker->ptitCountPtetTemp = 0;
			__leave;
		}

		//suspend current thread
		if (docker->Flag & DOCKER_ASK_SUSPENDED) {
			jmpAtAddress(docker->traceLoopTraceInstruction, docker->traceLoopTraceInstruction);
			docker->Flag &= ~DOCKER_ASK_SUSPENDED;
			__leave;
		}


		context = &docker->context;
		buffer = docker->traceLoopTraceInstruction;
		docker->memRip = docker->oldRip;
		docker->oldRip = (void*)((ULONG_PTR)docker->Rip & MAXDWORD64);
		docker->context.Rip = (DWORD64)((ULONG_PTR)docker->Rip & MAXDWORD64);

		do {


			//STEP 0 : get instruction
			if (!GetInstruction(docker->Rip, &instructionRead)) {
				success = FALSE;
				__leave;
			}

			//STEP 1 : check memory right
			memoryCheckValue = SingleStepHandlerMemoryCheck(&docker->memBasicInfo, docker->Rip, instructionRead.Length);
			if (memoryCheckValue == PAGE_EXECUTE_READWRITE) {
				docker->ptitCountPtetPasTemp++;
			}
			if (memoryCheckValue == PAGE_EXECUTE_READWRITE && context == NULL) {
				//break;
			}
			else if (memoryCheckValue == PAGE_NOACCESS && g_ExceptionCallback) {
				g_ExceptionCallback((EMULATOR_HANDLER*)docker, NULL, CALL_BACK_EXECUTION_ACCESS_VIOLATION);
				success = FALSE;
				__leave;
			}			
			else if (memoryCheckValue == PAGE_EXECUTE_READ) {
				docker->ptitCountPtetTemp++;
				if (docker->ptitCountPtetTemp > 100) {
					g_mmm.AreAllModuleInit = FALSE;
				}
			}

			//STEP 3 : instruction management
			if (instructionRead.UserData != NULL) {
				((BOOL(*)(
					_In_ DOCKER * pDocker,
					_In_opt_ THREAD_CONTEXT * pContext,
					_In_ INSTRUCTION_READ * pInstruction,
					_Inout_ OPCODE * buffer,
					_Inout_ DWORD * pBufferIndex,
					_In_ DWORD bufferLength
				)) instructionRead.UserData)(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength);
			}
			else if (!IsPrbINSTRUCTION(&instructionRead)) {
				instructionRead.UserData = ManageBasicInstruction;
				if (!ManageBasicInstruction(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}
			else if (instructionRead.IsJcc) {
				instructionRead.UserData = ManageJcc;
				if (!ManageJcc(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}
			else if (instructionRead.IsLoop) {
				instructionRead.UserData = ManageLoop;
				if (!ManageLoop(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}
			else if (instructionRead.IsRet) {
				instructionRead.UserData = ManageRet;
				if (!ManageRet(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}
			else if (instructionRead.IsJmp) {
				instructionRead.UserData = ManageJmp;
				if (!ManageJmp(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}
			else if (isSyscall(instructionRead.Opcode + instructionRead.EndOfPrefix)) {
				//__debugbreak();
				if (!ManageSyscall(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
				break;
			}
			else if (instructionRead.IsCall) {//After isSyscall
				instructionRead.UserData = ManageCall;
				if (!ManageCall(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}
			else {//if (instructionRead.IsRipRelative) At the end
				instructionRead.UserData = ManageRipRelative;
				if (!ManageRipRelative(docker, context, &instructionRead, buffer, &bufferIndex, bufferLength)) {
					break;
				}
			}

			if (g_EnableRangeInstructionAnalyseFunction(instructionRead.Opcode, instructionRead.Length)) {
				void * tmpRip = docker->Rip;
				docker->Rip = (void*)docker->context.Rip;


				CallBackExecute(docker, &instructionRead);

				docker->Rip = tmpRip;
			}

			docker->lastRip = (void*)docker->context.Rip;
			docker->context.Rip = (DWORD64)((ULONG_PTR)docker->Rip & MAXDWORD64);
			context = NULL;
		} while (FALSE);



		jmpRel32(buffer + bufferIndex, docker->traceLoopStart);
		FlushInstructionCache(GetCurrentProcess(), docker->traceLoopTraceInstruction, (SIZE_T)bufferIndex + (SIZE_T)5);
	}
	__finally {

		if (!success) {
			/*remote error handling*/
			DebugBreak();
		}

	}

	return;
}



BOOL IsPrbINSTRUCTION(INSTRUCTION_READ* instructionRead) {
	return instructionRead->IsCall || instructionRead->IsJcc || instructionRead->IsLoop || instructionRead->IsRet || instructionRead->IsRipRelative || instructionRead->IsJmp;
}

DWORD SingleStepHandlerMemoryCheck(PMEMORY_BASIC_INFORMATION memBasicInfo, BYTE* ptr, SIZE_T size) {
	DWORD tmp, tmp2 = PAGE_EXECUTE;
	if ((BYTE*)memBasicInfo->BaseAddress <= ptr &&
		(BYTE*)memBasicInfo->BaseAddress + memBasicInfo->RegionSize >= ptr + size) {
		if (memBasicInfo->Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ)) {
			return PAGE_EXECUTE_READ;
		}
		else if (memBasicInfo->Protect & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
			return PAGE_EXECUTE_READWRITE;
		}
		else {
			return PAGE_NOACCESS;
		}
	}
	else {
		if ((BYTE*)memBasicInfo->BaseAddress <= ptr &&
			(BYTE*)memBasicInfo->BaseAddress + memBasicInfo->RegionSize >= ptr) {
			tmp = memBasicInfo->Protect;
		}
		else {
			VirtualQuery(ptr, memBasicInfo, sizeof(MEMORY_BASIC_INFORMATION));
			tmp = memBasicInfo->Protect;
		}
		if (!((BYTE*)memBasicInfo->BaseAddress <= ptr + size &&
			(BYTE*)memBasicInfo->BaseAddress + memBasicInfo->RegionSize >= ptr + size)) {
			DWORD error = GetLastError();
			VirtualQuery(ptr + size - 1, memBasicInfo, sizeof(MEMORY_BASIC_INFORMATION));
			tmp2 = memBasicInfo->Protect;
			SetLastError(error);
		}
		if ((tmp & (PAGE_EXECUTE | PAGE_EXECUTE_READ)) &&
			(tmp2 & (PAGE_EXECUTE | PAGE_EXECUTE_READ))) {
			return PAGE_EXECUTE_READ;
		}
		else if ((tmp & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) &&
			(tmp2 & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))) {
			return PAGE_EXECUTE_READWRITE;
		}
		else {
			return PAGE_NOACCESS;
		}
	}
}
