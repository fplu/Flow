#include "FlowMain.h"

#define CEIL_TO_PAGE(a) (((QWORD)(a) + USN_PAGE_SIZE - 1)/USN_PAGE_SIZE)

extern void(*g_InstructionDefaultCallback)(EMULATOR_HANDLER*);
extern void(*(*g_GetInstructionCallback)(INSTRUCTION_READ*))(EMULATOR_HANDLER*);

extern OPCODE EndGoCallback;
extern OPCODE StartGoCallback;
extern OPCODE StartCodeGoCallback;
extern OPCODE StartCodeGoCallback_2;
extern OPCODE StartCodeGoCallback_Ret;
extern OPCODE StartCodeGoCallback_Jmp;
extern OPCODE StartCodeGoCallback_Call;
extern OPCODE StartCodeGoCallback_Syscall;
extern OPCODE StartCodeGoCallback_CallShadowStack;
extern OPCODE StartCodeGoCallback_RetShadowStack;

extern OPCODE MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_1;
extern OPCODE MOV_RSI_RDI_DISPLACEMENT_DOCKER_TRACE_LOOP_TRACE_INSTRUCTION_1;
extern OPCODE MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_1;
extern OPCODE MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_2;
extern OPCODE MOV_RSI_RDI_DISPLACEMENT_DOCKER_CALLBACK_2;
extern OPCODE MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_2;

extern OPCODE SAVE_RSI;
extern OPCODE SAVE_RDI;
extern OPCODE MOV_RSI_TLS_DOCKER;
extern OPCODE XCHG_RSI_TLS_TMP;
extern OPCODE LOAD_RSI;
extern OPCODE LOAD_RDI;
extern OPCODE JMP_TLS_TMP;

extern OPCODE SAVE_RSI_2;
extern OPCODE SAVE_RDI_2;
extern OPCODE MOV_RSI_TLS_DOCKER_2;
extern OPCODE XCHG_RSI_TLS_TMP_2;
extern OPCODE LOAD_RSI_2;
extern OPCODE LOAD_RDI_2;
extern OPCODE JMP_TLS_TMP_2;

extern OPCODE MOV_RAX_TO_TLS1_3;
extern OPCODE MOV_TLSPAGE_TO_RAX_3_1;
extern OPCODE MOV_RAX_TO_TLS2_3_1;
extern OPCODE MOV_TLS2_TO_RCX_3;
extern OPCODE MOV_RAX_TO_TLS2_3_2;
extern OPCODE MOV_TLSPAGE_TO_RAX_3_2;
extern OPCODE MOV_TLS1_TO_RAX_3;
extern OPCODE JMP_AT_TLS2_3;

extern OPCODE MOV_RAX_TO_TLS1_4;
extern OPCODE MOV_TLSPAGE_TO_RAX_4;
extern OPCODE MOV_TLS2_TO_RCX_4;
extern OPCODE MOV_RAX_TO_TLS2_4;

extern OPCODE MOV_RAX_TO_TLS1_5;
extern OPCODE MOV_TLSPAGE_TO_RAX_5_1;
extern OPCODE MOV_TLS2_TO_RCX_5;
extern OPCODE MOV_TLSPAGE_TO_RDX_5;
extern OPCODE MOV_RAX_TO_TLS2_5;
extern OPCODE MOV_TLSPAGE_TO_RAX_5_2;
extern OPCODE MOV_TLS3_TO_RCX_5;
extern OPCODE MOV_TLS1_TO_RAX_5;
extern OPCODE JMP_AT_TLS2_5;

extern OPCODE MOV_RAX_TO_TLS1_6;
extern OPCODE MOV_TLSPAGE_TO_RAX_6_1;
extern OPCODE MOV_TLS1_TO_RAX_6_1;
extern OPCODE MOV_RAX_TO_TLS2_6;
extern OPCODE MOV_TLSPAGE_TO_RAX_6_2;
extern OPCODE MOV_TLS1_TO_RAX_6_2;
extern OPCODE JMP_AT_TLS2_6;

extern OPCODE MOV_RAX_TO_TLS1_7;
extern OPCODE MOV_TLSPAGE_TO_RAX_7_1;
extern OPCODE MOV_RCX_TO_TLS3_7;
extern OPCODE MOV_TLSPAGE_TO_RAX_7_2;
extern OPCODE MOV_TLS1_TO_RAX_7;
extern OPCODE JMP_AT_TLS3_7;

extern OPCODE MOV_RAX_TO_TLS1_8;
extern OPCODE MOV_TLSPAGE_TO_RAX_8_1;
extern OPCODE MOV_TLS2_TO_RCX_8;
extern OPCODE MOV_RAX_TO_TLS3_8;
extern OPCODE MOV_TLSPAGE_TO_RAX_8_2;
extern OPCODE MOV_TLS1_TO_RAX_8;
extern OPCODE JMP_AT_TLS3_8;


MAPPED_MODULE_MANAGEMENT g_mmm = { 0 };
PMAPPED_PAGE* g_ppmp = NULL;
__declspec(thread) MAPPED_MODULE_PROCESSING g_mmp = {.BeingProcessed = FALSE};


/*
BOOL IsAddressInDataPage(PMAPPED_PAGE pPage, QWORD address) {
	MEMORY_BASIC_INFORMATION memoryBasicInformation = { 0 };
	DWORD i;

	for (i = 0; i < pPage->OldDatas.Length; i++) {
		if (address >= pPage->OldDatas.Pages[i].BaseAddress &&
			address < pPage->OldDatas.Pages[i].BaseAddress + pPage->OldDatas.Pages[i].RegionSize) {
			return TRUE;
		}
	}



	if (pPage->OldDatas.Length >= pPage->OldDatas.MaxLength) {
		return FALSE;
	}

	VirtualQuery(address, &memoryBasicInformation, sizeof(MEMORY_BASIC_INFORMATION));
	if (address < memoryBasicInformation.BaseAddress ||
		address >= (QWORD)memoryBasicInformation.BaseAddress + memoryBasicInformation.RegionSize) {
		return FALSE;
	}

	if ((PAGE_READONLY | PAGE_EXECUTE_READ) & memoryBasicInformation.Protect) {
		pPage->OldDatas.Pages[pPage->OldDatas.Length].BaseAddress = memoryBasicInformation.BaseAddress;
		pPage->OldDatas.Pages[pPage->OldDatas.Length].RegionSize = memoryBasicInformation.RegionSize;
		pPage->OldDatas.Length++;
		return TRUE;
	}

	return FALSE;
}*/

#define ceilToPage(a) ((((a) + USN_PAGE_SIZE - 1)/ USN_PAGE_SIZE) * USN_PAGE_SIZE)
#define CALLBACK_COUNT 8
#define DATA_COUNT 7

/*
	_In_ hModule : module to disassemble
	return analysedSize or (0 if fail)
*/
QWORD ModuleMappingMap(_In_ OPCODE * baseAddress, _In_ DWORD baseSize, _Inout_ MAPPED_PAGE * mappedModule, _In_ BOOL analyzeModule) {
	QWORD analysedSize = 0;
	MAPPED_PAGE res = { NULL };
	INSTRUCTION_READ instructionRead;
	DWORD i, modulePos;
	QWORD dest;
	//DWORD newPos;
	MAPPED_INSTRUCTION * moduleInstructions = NULL;
	DWORD moduleInstructionsLength;
	DWORD oldUnmappedIndex, unmappedIndex;

	MAPPED_INSTRUCTION * instructionMapped;
	QWORD newAddress, newAddressDelta;
	CALLBACK_LIST callbackList;
//	QWORD ** ppExitCallback, **ppCallback2, ** ppRetCallback, **ppJmpCallback, **ppCallCallback, **ppSyscallCallback;
	MEMORY_BASIC_INFORMATION memoryBasicInformation = { 0 };
	DWORD oldProtect;
	DWORD oldToIdLength;
	DWORD * oldToId = NULL;
	MAPPED_INSTRUCTION_MINIMAL_INFO* instructionsMinimal = NULL;
	DWORD instructionsMinimalLength;

	__try {
		g_mmp.OldAddress = baseAddress;
		g_mmp.OldSize = baseSize;
		g_mmp.BeingProcessed = TRUE;

		if (baseAddress == NULL || mappedModule == NULL) {
			analysedSize = 0;
			__leave;
		}

		//res.OldDatas.MaxLength = PAGE_RANGE_BUFFER_DEFAULT_LENGTH;
		res.IsModuleAnalysed = analyzeModule;
		res.CallInfoLength = 0;
		res.CallInfo = NULL;
		res.InstructionsMinimalLength = 0;

		//STEP 1 : Find the text section of the dll

		//if (((QWORD)baseAddress & 0xFFFFFFFFFFFF0000) == 0x00007ff8c7130000) {
			//__debugbreak();
			//}

		/**/
		do {
			VirtualQuery(baseAddress, &memoryBasicInformation, sizeof(MEMORY_BASIC_INFORMATION));
			(BYTE*)(baseAddress) += memoryBasicInformation.RegionSize;
			if (memoryBasicInformation.RegionSize >= baseSize) {
				analysedSize = 0;
				__leave;
			}
			analysedSize += memoryBasicInformation.RegionSize;
			baseSize -= (DWORD)memoryBasicInformation.RegionSize;
		} while (!(memoryBasicInformation.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ/* | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY*/)));

		/*res.IsModuleModifiable = ((memoryBasicInformation.Protect & (PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0);
		if (res.IsModuleModifiable ) {
			if (!VirtualProtect(memoryBasicInformation.BaseAddress, memoryBasicInformation.RegionSize, PAGE_EXECUTE_READ, &oldProtect)) {
				analysedSize = 0;
				__leave;
			}
		}*/
		//if (((QWORD)memoryBasicInformation.BaseAddress & 0xFFFFFFFFFFFF0000) == 0x00007ff8c7130000) {
			//	__debugbreak();
		//}

		res.OriginalProtect = memoryBasicInformation.Protect;
		res.OldCode = memoryBasicInformation.BaseAddress;
		//minus 16 to avoid buffer overflow if end of module is data
		res.OldCodeLength = (DWORD)memoryBasicInformation.RegionSize - 16;
		oldToIdLength = (DWORD)memoryBasicInformation.RegionSize;

		/*/

		res.OldCode = baseAddress;
		//minus 16 to avoid buffer overflow if end of module is data
		res.OldCodeLength = baseSize - 16;
		oldToIdLength = (DWORD)baseSize;

		/**/


		//STEP 2 : allocate and init instruction mask
		if ((oldToId = VirtualAlloc(NULL, oldToIdLength * sizeof(DWORD), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}
		for (i = 0; i < oldToIdLength; i++) {
			oldToId[i] = NO_LINKED_INSTRUCTION;//invalid value of instruction mask
		}

		//STEP 3 : count the number of instruction in the module and allocate space for it
		moduleInstructionsLength = 0;
		modulePos = 0;
		do {
			if (!GetInstruction(res.OldCode + modulePos, &instructionRead)) {
				modulePos++;
			}
			else {
				modulePos += instructionRead.Length;
			}
			moduleInstructionsLength++;
		} while (modulePos < res.OldCodeLength);

		if ((moduleInstructions = VirtualAlloc(NULL, moduleInstructionsLength * sizeof(MAPPED_INSTRUCTION), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}
		instructionsMinimalLength = moduleInstructionsLength;
		if ((instructionsMinimal = VirtualAlloc(NULL, instructionsMinimalLength * sizeof(MAPPED_INSTRUCTION_MINIMAL_INFO), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}

		//STEP 4 : Save all the instruction of the module
		modulePos = 0;
		for (i = 0; i < moduleInstructionsLength; i++) {
			if (!GetInstruction((const OPCODE*)res.OldCode + modulePos, &(moduleInstructions[i].AInstruction.Instruction))) {
				moduleInstructions[i].AInstruction.Instruction = NopInstruction();//set invalid instruction as nop instruction
				moduleInstructions[i].LinkedInstruction = NO_LINKED_INSTRUCTION;
				moduleInstructions[i].IsUnMapped = TRUE;//it is data or/and invalid opcode we do not need to map
			}
			else {
				//if (!isSyscall((moduleInstructions[i].Opcode + moduleInstructions[i].EndOfPrefix))) {
				oldToId[modulePos] = i;//we put valid instruction inside the mask if it is not a syscall
				//}
				moduleInstructions[i].LinkedInstruction = NO_LINKED_INSTRUCTION;
				moduleInstructions[i].IsUnMapped = FALSE;
			}
			instructionsMinimal[i].OldAddress = (QWORD)modulePos + (QWORD)res.OldCode;
			modulePos += moduleInstructions[i].Length;
		}

		//STEP 5 : link branch instruction with their dest (jump resolution)
		for (i = 0; i < moduleInstructionsLength; i++) {
			instructionMapped = moduleInstructions + i;

			dest = (QWORD)NULL;

			if (instructionMapped->IsJmp) {

				dest = (QWORD)GetJmpDest(
					instructionMapped->Opcode + instructionMapped->EndOfPrefix,
					instructionMapped->Rex,
					instructionMapped->Opcode,
					instructionMapped->Length,
					NULL,
					instructionMapped->AddressSize
				);
			}
			else if (instructionMapped->IsCall) {
				res.CallInfoLength++;

				dest = (QWORD)GetCallDest(
					instructionMapped->Opcode + instructionMapped->EndOfPrefix,
					instructionMapped->Rex,
					instructionMapped->Opcode,
					instructionMapped->Length,
					NULL,
					instructionMapped->AddressSize
				);
			}
			else if (instructionMapped->IsJcc || instructionMapped->IsLoop) {
				dest = (QWORD)GetJccOrLoopDest(
					instructionMapped->Opcode + instructionMapped->EndOfPrefix,
					instructionMapped->Opcode,
					instructionMapped->Length
				);
			}

			//if bound are correct we link the jump to the associated instruction, 
			//it could be NO_LINKED_INSTRUCTION which mean the jump destination is not yet resolvable or never use
			if (dest != (QWORD)NULL && dest >= (QWORD)res.OldCode && dest < (QWORD)res.OldCode + res.OldCodeLength) {
				instructionMapped->LinkedInstruction = oldToId[(QWORD)dest - (QWORD)res.OldCode];
			}
		}

		//STEP 6 : determine new size and RELATIVE position for the instruction
		res.NewCodeLength = 0;
		res.NewDataLength = 0;
		res.InstructionsLength = 0;
		for (i = 0; i < moduleInstructionsLength; i++) {
			instructionMapped = moduleInstructions + i;

			instructionMapped->NewAddress = (OPCODE*)(QWORD)res.NewCodeLength;

			instructionMapped->Callback = NULL;
			if (g_GetInstructionCallback) {
				instructionMapped->Callback = g_GetInstructionCallback(&instructionMapped->AInstruction.Instruction);
			}

			if (analyzeModule && 
				(g_InstructionDefaultCallback || instructionMapped->Callback)) {
				res.InstructionsMinimalLength++;
				res.NewCodeLength += 12;//Arg
				res.NewCodeLength += 6;//Callback
			}

			//Last INSTRUCTION (except if linked jump)
			if (!(instructionMapped->IsJmp) && i + 1 == moduleInstructionsLength) {
				res.NewCodeLength += GetMappedExitMappingSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (instructionMapped->IsJmp) {
				res.NewCodeLength += GetMappedJmpSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (instructionMapped->IsJcc) {
				res.NewCodeLength += GetMappedJccSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (instructionMapped->IsLoop) {
				res.NewCodeLength += GetMappedLoopSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (isSyscall((instructionMapped->Opcode + instructionMapped->EndOfPrefix))) {
				res.NewCodeLength += GetMappedSyscallSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (instructionMapped->IsCall) {
				res.NewCodeLength += GetMappedCallSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (instructionMapped->IsRet) {
	//			__debugbreak();
				res.NewCodeLength += GetMappedRetSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else if (instructionMapped->IsRipRelative) {
				res.NewCodeLength += GetMappedRipRelSize(&res, instructionMapped, &res.InstructionsLength);
			}
			else {
				res.NewCodeLength += GetMappedOthersSize(&res, instructionMapped, &res.InstructionsLength);
			}
		}

		//STEP 7 : allocation of the space for the new code
		if ((res.Instructions = VirtualAlloc(NULL, (res.InstructionsLength + (SIZE_T)1)* sizeof(MAPPED_INSTRUCTION_MINIMAL_INFO), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}

		if (res.CallInfoLength != 0 && (res.CallInfo = VirtualAlloc(NULL, (res.CallInfoLength) * sizeof(PAGE_CALL_INFO), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}
		res.CallInfoLength = 0;

		if (res.InstructionsMinimalLength != 0 && (res.InstructionsMinimal = VirtualAlloc(NULL, (res.InstructionsMinimalLength) * sizeof(MAPPED_INSTRUCTION_MINIMAL_INFO), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}
		res.InstructionsMinimalLength = 0;

		res.OldCodeMaskLength = oldToIdLength;
		if ((res.OldCodeMask = VirtualAlloc(NULL, res.OldCodeMaskLength * sizeof(DWORD), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}

		//We need to make everything in one allocation to ensure that SaveZone and IndexZone are close to NewCode (less than 2Go)
		if ((res.NewData = VirtualAlloc(NULL, res.NewCodeLength + ceilToPage(((QWORD)res.NewDataLength + (QWORD)CALLBACK_COUNT + (QWORD)DATA_COUNT) * sizeof(QWORD)) + ceilToPage(&EndGoCallback - &StartCodeGoCallback), MEM_COMMIT, PAGE_EXECUTE_READWRITE)) == NULL) {
			analysedSize = 0;
			__leave;
		}


		//data section
		callbackList.ppExit = (QWORD**)(res.NewData + res.NewDataLength);
		callbackList.ppCallback = (QWORD**)(res.NewData + res.NewDataLength + 1);
		callbackList.ppRet = (QWORD**)(res.NewData + res.NewDataLength + 2);
		callbackList.ppRet2 = (QWORD**)(res.NewData + res.NewDataLength + 3);
		callbackList.ppJmp= (QWORD**)(res.NewData + res.NewDataLength + 4);
		callbackList.ppCall = (QWORD**)(res.NewData + res.NewDataLength + 5);
		callbackList.ppCall2 = (QWORD**)(res.NewData + res.NewDataLength + 6);
		callbackList.ppSyscall = (QWORD**)(res.NewData + res.NewDataLength + 7);

		//code section
		*callbackList.ppExit = (QWORD*) (((QWORD)res.NewData) + ceilToPage(((QWORD)res.NewDataLength + (QWORD)CALLBACK_COUNT + (QWORD)DATA_COUNT) * sizeof(QWORD)));
		*callbackList.ppCallback = (QWORD*) (((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_2 - (QWORD)&StartCodeGoCallback);
		*callbackList.ppRet = (QWORD*)(((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_Ret - (QWORD)&StartCodeGoCallback);
		*callbackList.ppRet2 = (QWORD*)(((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_RetShadowStack - (QWORD)&StartCodeGoCallback);
		*callbackList.ppJmp = (QWORD*)(((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_Jmp - (QWORD)&StartCodeGoCallback);
		*callbackList.ppCall = (QWORD*)(((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_Call - (QWORD)&StartCodeGoCallback);
		*callbackList.ppCall2 = (QWORD*)(((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_CallShadowStack - (QWORD)&StartCodeGoCallback);
		*callbackList.ppSyscall = (QWORD*)(((QWORD)*callbackList.ppExit) + (QWORD)&StartCodeGoCallback_Syscall - (QWORD)&StartCodeGoCallback);
//		*ppSyscallCallback = *ppExitCallback;
		if (g_SyscallDefaultCallback == NULL) {
			*callbackList.ppSyscall = *callbackList.ppExit;
		}
		


		res.NewCode  = (OPCODE*)(((QWORD)res.NewData) + ceilToPage((QWORD)((QWORD)res.NewDataLength + (QWORD)CALLBACK_COUNT + (QWORD)DATA_COUNT) * sizeof(QWORD)) + ceilToPage(&EndGoCallback - &StartCodeGoCallback));

		MovMyAssembly(
			&StartGoCallback,
			&StartCodeGoCallback,
			(DWORD)((QWORD)&EndGoCallback - (QWORD)&StartGoCallback),
			(QWORD[]) {
				(QWORD)res.Instructions, (QWORD)res.InstructionsMinimal, 
				(QWORD) &g_mmm, (QWORD) &g_ppmp, 
				(QWORD) &g_SyscallCallback, (QWORD) &g_SyscallCallbackLength,
				(QWORD) res.CallInfo
			},
			DATA_COUNT,
			(OPCODE*)*callbackList.ppExit
		);


		//STEP 8 set the instruction newAddress to absolute from relative
		for (i = 0; i < moduleInstructionsLength; i++) {
			moduleInstructions[i].NewAddress += (QWORD)res.NewCode;
		}
		for (i = 0; i < moduleInstructionsLength; i++) {
			if(moduleInstructions[i].LinkedInstruction != NO_LINKED_INSTRUCTION) {
				moduleInstructions[i].Dest = (QWORD)moduleInstructions[moduleInstructions[i].LinkedInstruction].NewAddress;
			}
		}

		//Set OldCodeMask
		for (i = 0; i < res.OldCodeMaskLength; i++) {
			res.OldCodeMask[i] = NO_LINKED_INSTRUCTION;//invalid value of instruction mask
		}

		//DebugBreak();
		unmappedIndex = oldUnmappedIndex = 0;
		modulePos = 0;
		res.NewDataLength = 0;
		newAddress = (QWORD)res.NewCode;
		for (i = 0; i < moduleInstructionsLength; i++) {
			/*if (instructionsMinimal[i].OldAddress == 0x00007ff8c7134060) {
				__debugbreak();
			}*/
			newAddressDelta = 0;
			instructionMapped = moduleInstructions + i;

			instructionsMinimal[i].NewAddress = newAddress;
			instructionsMinimal[i].Callback = instructionMapped->Callback;

			if (analyzeModule &&
				(g_InstructionDefaultCallback || instructionMapped->Callback)) {
				res.InstructionsMinimal[res.InstructionsMinimalLength] = instructionsMinimal[i];

				movDwordToTls(newAddress + newAddressDelta, g_TlsIdArg, res.InstructionsMinimalLength);
				newAddressDelta += 12;
				jmpAtAddress(newAddress + newAddressDelta, callbackList.ppCallback);
				newAddressDelta += 6;

				res.InstructionsMinimalLength++;

			}

			//Unmappable instruction :
			//Last INSTRUCTION (except if linked jump)
			res.Instructions[unmappedIndex] = instructionsMinimal[i];// moduleInstructions[i];
			if (!(instructionMapped->IsJmp) && i + 1 == moduleInstructionsLength) {
				newAddressDelta += WriteMappedExitMapping(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), (void(*)(void))(void*)callbackList.ppExit, g_TlsIdArg, &unmappedIndex);
			}
			else if (instructionMapped->IsJmp) {
				newAddressDelta += WriteMappedJmp(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else if (instructionMapped->IsJcc) {
				newAddressDelta += WriteMappedJcc(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else if (instructionMapped->IsLoop) {
				newAddressDelta += WriteMappedLoop(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else if (instructionMapped->IsRet) {
				//__debugbreak();
				newAddressDelta += WriteMappedRet(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else if (isSyscall((instructionMapped->Opcode + instructionMapped->EndOfPrefix))) {
				newAddressDelta += WriteMappedSyscall(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else if (instructionMapped->IsCall) {//Unmappable instruction
				newAddressDelta += WriteMappedCall(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else if (instructionMapped->IsRipRelative) {
				newAddressDelta += WriteMappedRipRel(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}
			else {
				newAddressDelta += WriteMappedOthers(&res, instructionMapped, (OPCODE*)(newAddress + newAddressDelta), &callbackList, g_TlsIdArg, &unmappedIndex);
			}

//			if (unmappedIndex == oldUnmappedIndex) {
				res.OldCodeMask[modulePos] = (DWORD)((QWORD)newAddress - (QWORD)res.NewCode);
//			}

			oldUnmappedIndex = unmappedIndex;
			newAddress += newAddressDelta;
			modulePos += instructionMapped->Length;
		}


		//VirtualProtect(res.IndexZone, res.NewCodeLength + USN_PAGE_SIZE + USN_PAGE_SIZE, PAGE_EXECUTE_READ, &oldProtect);
		VirtualProtect(res.NewData, ceilToPage(((QWORD)res.NewDataLength + (QWORD)CALLBACK_COUNT + (QWORD)DATA_COUNT) * sizeof(QWORD)), PAGE_READONLY, &oldProtect);
		VirtualProtect(res.NewCode, res.NewCodeLength, PAGE_EXECUTE_READ, &oldProtect);

		//__debugbreak();
		//	DebugBreak();
	}
	__finally {
		g_mmp.BeingProcessed = FALSE;

		if (moduleInstructions) {
			VirtualFree(moduleInstructions, 0, MEM_RELEASE);
		}
		if (oldToId) {
			VirtualFree(oldToId, 0, MEM_RELEASE);
		}
		if (instructionsMinimal) {
			VirtualFree(instructionsMinimal, 0, MEM_RELEASE);
			//res.InstructionsMinimal = instructionsMinimal;
		}
		if (analysedSize != 0) {
			*mappedModule = res;
		}
	}


	return analysedSize;
}


void ModuleMappingOnNewModuleLoad(
	_In_     ULONG NotificationReason,
	_In_     PLDR_DLL_NOTIFICATION_DATA NotificationData,
	_In_opt_ PVOID Context) {//each time a module is load
	if (LDR_DLL_NOTIFICATION_REASON_LOADED == NotificationReason) {
		g_mmm.AreAllModuleInit = FALSE;
	}
	else if(LDR_DLL_NOTIFICATION_REASON_UNLOADED == NotificationReason) {
		__debugbreak();
	}
}


#pragma warning(push)
#pragma warning(disable: 4739)
BOOL ModuleMappingInit(_In_ PVOID callback_) {
	PLDR_REGISTER_NOTIFICATION LdrRegisterDllNotification;
	DWORD oldProtect;
	BOOL success = TRUE;
	HMODULE hNtdll;

	__try {
		g_mmm.Element = NULL;
		//		DebugBreak();
		if ((hNtdll = GetModuleHandle(_T("ntdll.dll"))) == NULL) {
			success = FALSE;
			__leave;
		}

		if ((LdrRegisterDllNotification = (PLDR_REGISTER_NOTIFICATION)GetProcAddress(hNtdll, "LdrRegisterDllNotification")) == NULL) {
			success = FALSE;
			__leave;
		}

		if (LdrRegisterDllNotification(0, ModuleMappingOnNewModuleLoad, NULL, &(g_mmm.Cookie)) != STATUS_SUCCESS) {
			success = FALSE;
			__leave;
		}

		if ((g_ppmp = VirtualAlloc(NULL, 0x1000000 * sizeof(PMAPPED_PAGE), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			success = FALSE;
			__leave;
		}


		if (!VirtualProtect(&StartCodeGoCallback, &EndGoCallback - &StartCodeGoCallback, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			success = FALSE;
			__leave;
		}

		movRdiImm32DisplacementRsi((&MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_1), offsetof(DOCKER, mappedInstruction));
		movRsiRdiImm32Displacement((&MOV_RSI_RDI_DISPLACEMENT_DOCKER_TRACE_LOOP_TRACE_INSTRUCTION_1), offsetof(DOCKER, traceLoopTraceInstruction));
		movRdiImm32DisplacementRsi((&MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_1), offsetof(DOCKER, instructionId));
		movRdiImm32DisplacementRsi((&MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_2), offsetof(DOCKER, mappedInstruction));
		movRsiRdiImm32Displacement((&MOV_RSI_RDI_DISPLACEMENT_DOCKER_CALLBACK_2), offsetof(DOCKER, callbackBaseAddress));
		movRdiImm32DisplacementRsi((&MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_2), offsetof(DOCKER, instructionId));
		
		movRsiToTls((&SAVE_RSI), g_TlsIdStoreR1);
		movRdiToTls((&SAVE_RDI), g_TlsIdStoreR2);
		movTlsToRsi((&MOV_RSI_TLS_DOCKER), g_TlsIdDocker);
		xchgRsiToTls((&XCHG_RSI_TLS_TMP), g_TlsIdArg);
		movTlsToRsi((&LOAD_RSI), g_TlsIdStoreR1);
		movTlsToRdi((&LOAD_RDI), g_TlsIdStoreR2);
		jmpAtTls((&JMP_TLS_TMP), g_TlsIdArg);

		movRsiToTls((&SAVE_RSI_2), g_TlsIdStoreR1);
		movRdiToTls((&SAVE_RDI_2), g_TlsIdStoreR2);
		movTlsToRsi((&MOV_RSI_TLS_DOCKER_2), g_TlsIdDocker);
		xchgRsiToTls((&XCHG_RSI_TLS_TMP_2), g_TlsIdArg);
		movTlsToRsi((&LOAD_RSI_2), g_TlsIdStoreR1);
		movTlsToRdi((&LOAD_RDI_2), g_TlsIdStoreR2);
		jmpAtTls((&JMP_TLS_TMP_2), g_TlsIdArg);

		movRaxToTls((&MOV_RAX_TO_TLS1_3), g_TlsIdStoreR1);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_3_1), g_TlsIdStorePage);
		movRaxToTls((&MOV_RAX_TO_TLS2_3_1), g_TlsIdStoreR2);
		movTlsToRcx((&MOV_TLS2_TO_RCX_3), g_TlsIdStoreR2);
		movRaxToTls((&MOV_RAX_TO_TLS2_3_2), g_TlsIdStoreR2);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_3_2), g_TlsIdStorePage);
		movTlsToRax((&MOV_TLS1_TO_RAX_3), g_TlsIdStoreR1);
		jmpAtTls((&JMP_AT_TLS2_3), g_TlsIdStoreR2);

		movRaxToTls((&MOV_RAX_TO_TLS1_4), g_TlsIdStoreR1);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_4), g_TlsIdStorePage);
		movTlsToRcx((&MOV_TLS2_TO_RCX_4), g_TlsIdStoreR2);
		movRaxToTls((&MOV_RAX_TO_TLS2_4), g_TlsIdStoreR2);


		movRaxToTls((&MOV_RAX_TO_TLS1_5), g_TlsIdStoreR1);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_5_1), g_TlsIdStorePage);
		movTlsToRcx((&MOV_TLS2_TO_RCX_5), g_TlsIdStoreR2);
		movTlsToRdx((&MOV_TLSPAGE_TO_RDX_5), g_TlsIdStorePage);
		movRaxToTls((&MOV_RAX_TO_TLS2_5), g_TlsIdStoreR2);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_5_2), g_TlsIdStorePage);
		movTlsToRcx((&MOV_TLS3_TO_RCX_5), g_TlsIdStoreR3);
		movTlsToRax((&MOV_TLS1_TO_RAX_5), g_TlsIdStoreR1);
		jmpAtTls((&JMP_AT_TLS2_5), g_TlsIdStoreR2);


		movRaxToTls((&MOV_RAX_TO_TLS1_6), g_TlsIdStoreR1);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_6_1), g_TlsIdStorePage);
		movTlsToRax((&MOV_TLS1_TO_RAX_6_1), g_TlsIdStoreR1);
		movRaxToTls((&MOV_RAX_TO_TLS2_6), g_TlsIdStoreR2);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_6_2), g_TlsIdStorePage);
		movTlsToRax((&MOV_TLS1_TO_RAX_6_2), g_TlsIdStoreR1);
		jmpAtTls((&JMP_AT_TLS2_6), g_TlsIdStoreR2);

		movRaxToTls((&MOV_RAX_TO_TLS1_7), g_TlsIdStoreR1);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_7_1), g_TlsIdStorePage);
		movRcxToTls((&MOV_RCX_TO_TLS3_7), g_TlsIdStoreR3);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_7_2), g_TlsIdStorePage);
		movTlsToRax((&MOV_TLS1_TO_RAX_7), g_TlsIdStoreR1);
		jmpAtTls((&JMP_AT_TLS3_7), g_TlsIdStoreR3);

		movRaxToTls((&MOV_RAX_TO_TLS1_8), g_TlsIdStoreR1);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_8_1), g_TlsIdStorePage);
		movTlsToRcx((&MOV_TLS2_TO_RCX_8), g_TlsIdStoreR2);
		movRaxToTls((&MOV_RAX_TO_TLS3_8), g_TlsIdStoreR3);
		movTlsToRax((&MOV_TLSPAGE_TO_RAX_8_2), g_TlsIdStorePage);
		movTlsToRax((&MOV_TLS1_TO_RAX_8), g_TlsIdStoreR1);
		jmpAtTls((&JMP_AT_TLS3_8), g_TlsIdStoreR3);

		if (!VirtualProtect(&StartCodeGoCallback, &EndGoCallback - &StartCodeGoCallback, oldProtect, &oldProtect)) {
			success = FALSE;
			__leave;
		}

		g_mmm.ElementMaxLength = USN_PAGE_SIZE;
		g_mmm.Callback = callback_;
		if ((g_mmm.WastedElement = VirtualAlloc(NULL, g_mmm.ElementMaxLength * sizeof(MAPPED_PAGE), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			success = FALSE;
			__leave;
		}

		if ((g_mmm.Element = VirtualAlloc(NULL, g_mmm.ElementMaxLength * sizeof(MAPPED_PAGE), MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			success = FALSE;
			__leave;
		}
		g_mmm.WastedElementLength = 0;
		g_mmm.ElementLength = 0;
		//__debugbreak();
		g_mmm.AreAllModuleInit = FALSE;
		g_mmm.SynchronizationByte = 0;

	}
	__finally {

	}

	return success;
}
#pragma warning(pop) 


void ModuleMappingMapPage(OPCODE * baseAddress, DWORD baseSize) {
	QWORD j;
	DWORD i;
	BOOLEAN alreadyMapped = FALSE;
	DWORD wait;

	wait = InterlockedBitTestAndSetAcquire(&(g_mmm.SynchronizationByte), 0);//wait and take lock
	if (wait) {
		return;
	}


	for (i = 0; i < g_mmm.ElementLength; i++) {
		if (g_mmm.Element[i].OldCode <= baseAddress + baseSize&&
			g_mmm.Element[i].OldCode + g_mmm.Element[i].OldCodeLength >= baseAddress) {
			alreadyMapped = TRUE;
		}
	}


	if (alreadyMapped) {
		g_mmm.SynchronizationByte = 0;
		return;
	}

	if (g_mmm.ElementLength >= g_mmm.ElementMaxLength) {
		__debugbreak();
		g_mmm.SynchronizationByte = 0;
		return;
	}

	if ((ModuleMappingMap(
		baseAddress,
		baseSize,
		g_mmm.Element + g_mmm.ElementLength,
		g_EnableRangeInstructionAnalyse(baseAddress, baseSize))) != 0
		) {

		if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {
			for (j = (QWORD)g_mmm.Element[i].OldCode; j < (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength; j++) {
				g_ppmp(j) = g_mmm.Element + i;
			}
		}
		g_mmm.ElementLength++;
	}

	g_mmm.SynchronizationByte = 0;
}


BOOL ModuleMappingMapAllUnmappedModule(void) {
	static BOOLEAN* MappedRegionMask = NULL;
	static DWORD MappedRegionMaskLength = 0;

	QWORD bornMin;
	QWORD bornMax;

	BOOL success = TRUE;
	//HANDLE hModuleSnap = NULL;
	HMODULE hMods[1024];
	DWORD cbNeeded;
	MODULEINFO moduleInfo;

	//	MODULEENTRY32 moduleEntry;
	DWORD i;
	QWORD j;
	QWORD k;
	QWORD l;
	BOOL wait;
	BOOL find;
	QWORD analysedSize, deltaAnalysedSize;
	__try {
		if (g_mmm.AreAllModuleInit == TRUE) {
			__leave;

		}

		wait = InterlockedBitTestAndSetAcquire(&(g_mmm.SynchronizationByte), 0);//wait and take lock
		if (wait) {
			__leave;
		}

		g_mmm.AreAllModuleInit = TRUE;
		if (!EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
			success = FALSE;
			g_mmm.SynchronizationByte = 0;//reset lock
			__leave;
		}
		//__debugbreak();

		for (k = 0; k < (cbNeeded / sizeof(HMODULE)); k++) {
			/*if (g_mmm.ElementLength >= g_mmm.ElementMaxLength) {
				__debugbreak();
				g_mmm.SynchronizationByte = 0;//reset lock
				__leave;
			}*/

			if (!GetModuleInformation(GetCurrentProcess(), hMods[k], &moduleInfo, sizeof(MODULEINFO))) {
				success = FALSE;
				g_mmm.SynchronizationByte = 0;//reset lock
				__leave;
			}

			
			if (!g_EnableRangeInstructionInstrumentation((OPCODE*)moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage)) {
				break;
			}

			if (MappedRegionMaskLength < CEIL_TO_PAGE(moduleInfo.SizeOfImage)) {
				if (MappedRegionMask != NULL) {
					VirtualFree(MappedRegionMask, 0, MEM_RELEASE);
				}
				MappedRegionMaskLength = CEIL_TO_PAGE(moduleInfo.SizeOfImage);
				if ((MappedRegionMask = VirtualAlloc(NULL, MappedRegionMaskLength, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) == NULL) {
					success = FALSE;
					g_mmm.SynchronizationByte = 0;//reset lock
					__leave;
				}
			}

			for (i = 0; i < CEIL_TO_PAGE(moduleInfo.SizeOfImage); i++) {
				MappedRegionMask[i] = FALSE;
			}

			find = FALSE;

			//only one loop, we do not need something very precise, the job we be finished inside 
			for (i = 0; i < g_mmm.ElementLength; i++) {

				if (g_mmm.Element[i].OldCode < (OPCODE*)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage &&
					g_mmm.Element[i].OldCode + g_mmm.Element[i].OldCodeLength > (OPCODE*)moduleInfo.lpBaseOfDll) {

					//prise des bornes les plus grandes communes entre les deux ensembles
					bornMin = (QWORD)max(g_mmm.Element[i].OldCode, (OPCODE*)moduleInfo.lpBaseOfDll);
					bornMax = (QWORD)min(g_mmm.Element[i].OldCode + g_mmm.Element[i].OldCodeLength, (OPCODE*)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage);

					//reduction des bornes à l'origine
					bornMin -= (QWORD)moduleInfo.lpBaseOfDll;
					bornMax -= (QWORD)moduleInfo.lpBaseOfDll;

					//réduction des bornes à la granularity (PAGE_SIZE)
					bornMin = CEIL_TO_PAGE(bornMin);
					bornMax = CEIL_TO_PAGE(bornMax);

					//__debugbreak();
					for (l = bornMin; l < bornMax; l++) {
						MappedRegionMask[l] = TRUE;
					}
				}

			}
			if (find) {
				break;
			}
			for (l = 0; l < CEIL_TO_PAGE(moduleInfo.SizeOfImage); l++) {
				if (MappedRegionMask[l] == TRUE) {
					//__debugbreak();
					continue;
				}
				bornMin = l;
				for (l++ ; l < CEIL_TO_PAGE(moduleInfo.SizeOfImage); l++) {
					if (MappedRegionMask[l] == TRUE) {
						break;
					}
				}
				bornMax = l - bornMin;

				bornMin *= USN_PAGE_SIZE;
				bornMax *= USN_PAGE_SIZE;
				bornMin += (QWORD)moduleInfo.lpBaseOfDll;

				//__debugbreak();
				analysedSize = 0;
				do {

					if (g_mmm.ElementLength >= g_mmm.ElementMaxLength) {
						__debugbreak();
						g_mmm.SynchronizationByte = 0;//reset lock
						__leave;
					}


					deltaAnalysedSize = ModuleMappingMap(
						(OPCODE*)(bornMin + analysedSize),
						(DWORD)(bornMax - analysedSize),
						g_mmm.Element + g_mmm.ElementLength,
						g_EnableRangeInstructionAnalyse(moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage)
					);
					if (deltaAnalysedSize == 0) {
						break;
					}

					//__debugbreak();
					if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {
						for (j = (QWORD)g_mmm.Element[i].OldCode; j < (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength; j++) {
							g_ppmp(j) = g_mmm.Element + i;
						}
					}

					

					analysedSize += deltaAnalysedSize;
					g_mmm.ElementLength++;
				//	break;
				} while (bornMax - analysedSize > 0);
				//break;
			}


		}
		//__debugbreak();
		g_mmm.SynchronizationByte = 0;//reset lock
	}
	__finally {
		if (!success) {
			DebugBreak();
		}
	}

	return success;
}

QWORD ModuleMappingGetNewAddressFromOldAddress(_Inout_ volatile DWORD* pFlag, _In_ QWORD oldAddress, _In_ BOOL sync) {
	QWORD res = (QWORD)NULL;
	DWORD i;
	//QWORD j;
	PMAPPED_PAGE pMappedPage;
	static MEMORY_BASIC_INFORMATION memoryBasicInformation = { 0 };

	/*	if (sync) {
			while (*(volatile LONG*)&g_mmm.SynchronizationByte != 0);
		}*/

	if ((oldAddress >> 36) == 0x7FF && (pMappedPage = g_ppmp(oldAddress)) != NULL) {
		return GetNewModuleAddressFromOldModuleAddress(pMappedPage, oldAddress);
	}

	for (i = 0; i < g_mmm.ElementLength; i++) {
		if ((QWORD)g_mmm.Element[i].OldCode <= oldAddress && (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength > oldAddress) {
			return GetNewModuleAddressFromOldModuleAddress(g_mmm.Element + i, oldAddress);
		}
	}

	return (QWORD)NULL;
	/*if (pFlag == NULL) {
		return NULL;
	}

	DWORD error = GetLastError();

//	memoryBasicInformation.RegionSize /= 10;
	if (oldAddress >= memoryBasicInformation.BaseAddress && oldAddress < (QWORD)memoryBasicInformation.BaseAddress + memoryBasicInformation.RegionSize) {
		return NULL;
	}

	if (!VirtualQuery(oldAddress, &memoryBasicInformation, sizeof(MEMORY_BASIC_INFORMATION))) {
		SetLastError(error);
		return NULL;
	}
	
	if (!(memoryBasicInformation.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ))) {
		SetLastError(error);
		return NULL;
	}


	do {
		if ((volatile)*pFlag & DOCKER_ASK_SUSPENDED) {
			return NULL;
		}
		wait = InterlockedBitTestAndSetAcquire(&(g_mmm.SynchronizationByte), 0);//wait and take lock
	} while (wait);

	if (g_mmm.ElementLength >= g_mmm.ElementMaxLength) {
		__debugbreak();
		InterlockedBitTestAndResetRelease(&(g_mmm.SynchronizationByte), 0);//free lock
		return NULL;
	}

	//memoryBasicInformation.BaseAddress, memoryBasicInformation.RegionSize
	//oldAddress, 0
	if (MapModule(memoryBasicInformation.BaseAddress, memoryBasicInformation.RegionSize, g_mmm.Element + g_mmm.ElementLength, TRUE)) {
		if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {
			for (j = g_mmm.Element[g_mmm.ElementLength].OldCode; j < g_mmm.Element[g_mmm.ElementLength].OldCode + g_mmm.Element[g_mmm.ElementLength].OldCodeLength; j++) {
				g_ppmp(j) = g_mmm.Element + g_mmm.ElementLength;
			}
		}
		g_mmm.ElementLength++;

		g_mmm.SynchronizationByte = 0;//reset lock
		SetLastError(error);
		InterlockedBitTestAndResetRelease(&(g_mmm.SynchronizationByte), 0);//free lock
	}
	else {
		g_mmm.SynchronizationByte = 0;//reset lock
		SetLastError(error);
		InterlockedBitTestAndResetRelease(&(g_mmm.SynchronizationByte), 0);//free lock
		return NULL;
	}

	if ((oldAddress >> 36) == 0x7FF && (pMappedPage = g_ppmp(oldAddress)) != NULL) {
		return GetNewModuleAddressFromOldModuleAddress(pMappedPage, oldAddress);
	}

	for (i = 0; i < g_mmm.ElementLength; i++) {
		if ((QWORD)g_mmm.Element[i].OldCode <= oldAddress && (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength > oldAddress) {
			return GetNewModuleAddressFromOldModuleAddress(g_mmm.Element + i, oldAddress);
		}
	}

	return NULL;*/
}



/*
	You cannot suspend current thread using this function
	You can only suspend analysed thread using this function
	You can't suspend thread which were suspended externally
*/
BOOL FlowSuspendThread(DWORD threadId) {
	BOOL success = TRUE;
	DWORD i, j;
	DOCKER* docker = NULL;
	CONTEXT context;
	QWORD rip = 0;
	BOOLEAN find;
	LONG wait;
	PLONG pSync = NULL;
	HANDLE hThread = NULL;

	__try {
		//STEP 1 : we ensure that we are not stopping the current thread
		if (GetCurrentThreadId() == threadId) {
			success = FALSE;
			__leave;
		}
		//STEP 2 : we find the docker of the concerned thread
		for (i = 0; i < g_dockerThreadIdLength; i++) {
			if (threadId == g_dockerThreadId[i].ThreadId) {
				docker = g_dockerThreadId[i].Docker;
				pSync = &g_dockerThreadId[i].Sync;
				break;
			}
		}
		//if we do not find a docker we exit
		if (docker == NULL) {
			success = FALSE;
			__leave;
		}

		//STEP 3 : we take the lock on the remote docker
		do {
			//if he is already being notified by another instance we exit
			if (docker->Flag & DOCKER_ASK_SUSPENDED) {
				__leave;
			}
			wait = InterlockedBitTestAndSetAcquire(pSync, 0);//wait and take lock
		} while (wait);

		//STEP 4 : we get access on the remote thread
		hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, threadId);
		if (hThread == NULL) {
			success = FALSE;
			__leave;
		}


		//STEP 5 : we stop the remote docker
		//we alert the remote docker that it is being stopped
		docker->Flag |= DOCKER_ASK_SUSPENDED;//add handling
		docker->SuspendCount++;
		do {
			//we stop the thread
			SuspendThread(hThread);
			//get the context of the thread
			context.ContextFlags = CONTEXT_CONTROL;
			GetThreadContext(hThread, &context);
			//get the next/current instruction to be executed in the remote context
			//we get the current module being executed
			find = FALSE;
			for (i = 0; i < g_mmm.ElementLength; i++) {
				if ((QWORD)context.Rip >= (QWORD)g_mmm.Element[i].NewCode && (QWORD)context.Rip <= (QWORD)g_mmm.Element[i].NewCode + (QWORD)g_mmm.Element[i].NewCodeLength) {
					//we get the next instruction being executed
					for (j = 0; j < g_mmm.Element[i].OldCodeMaskLength; j++) {
						if ((QWORD)g_mmm.Element[i].OldCodeMask[j] + (QWORD)g_mmm.Element[i].NewCode == (QWORD)context.Rip) {
							rip = (QWORD)(g_mmm.Element[i].OldCode + j);
							find = TRUE;
							break;
						}
					}
					break;
				}
			}

			//if docker->Flag is cleared and we are executing the infinite loop
			if (!(docker->Flag & DOCKER_ASK_SUSPENDED) && ((QWORD)context.Rip == (QWORD)docker->traceLoopTraceInstruction || (QWORD)context.Rip == (QWORD)docker->traceLoopTraceInstruction + 6)) {
				rip = (QWORD)docker->Rip;
			}

			//if we find the original value of rip it also mean that the current context of the remote thread is valid, we can exit without resume
			if (rip != 0) {
				break;
			}

			//if we cannot restore the original context of the thread we simply wait a little until a better occasion shows itself
			if (ResumeThread(hThread) != 0) {
				success = FALSE;
				__leave;
			}
			Sleep(1);
		} while (1);

		//we simply change the value of rip and clear the request
		docker->Flag &= ~DOCKER_ASK_SUSPENDED;
		context.Rip = rip;
		SetThreadContext(hThread, &context);


	}
	__finally {
		if (docker != NULL && pSync != NULL) {
			*pSync = 0;
		}
		if (hThread != NULL) {
			CloseHandle(hThread);
		}
	}
	return success;
}



/*
	You can only resume thread which were suspended using FlowResumeThread
*/
BOOL FlowResumeThread(DWORD threadId) {
	BOOL success = TRUE;
	DWORD i;
	DOCKER* docker = NULL;
	CONTEXT context;
	PLONG pSync = NULL;
	LONG wait;
	HANDLE hThread = NULL;
	__try {
	
		//STEP 1 : we verify that is a thread we can handle
		for (i = 0; i < g_dockerThreadIdLength; i++) {
			if (threadId == g_dockerThreadId[i].ThreadId) {
				docker = g_dockerThreadId[i].Docker;
				pSync = &g_dockerThreadId[i].Sync;
				break;
			}
		}
		if (docker == NULL) {
			success = FALSE;
			__leave;
		}

		//STEP 2 : we take the lock if no one is trying to suspend the thread
		do {
			if (docker->Flag & DOCKER_ASK_SUSPENDED) {
				__leave;
			}
			wait = InterlockedBitTestAndSetAcquire(pSync, 0);//wait and take lock
		} while (wait);


		//STEP 3 : we get access on the remote thread
		hThread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT, FALSE, threadId);
		if (hThread == NULL) {
			success = FALSE;
			__leave;
		}

		//STEP 4 : we restore the rip to start the emulation again
		context.ContextFlags = CONTEXT_CONTROL;
		GetThreadContext(hThread, &context);
		docker->Rip = (void*)context.Rip;
		context.Rip = (DWORD64)docker->traceLoopStart;
		SetThreadContext(hThread, &context);
		ResumeThread(hThread);
		docker->SuspendCount--;

	}
	__finally {
		if (docker != NULL && pSync != NULL) {
			docker->Flag &= ~DOCKER_ASK_SUSPENDED;
			*pSync = 0;
		}
		if (hThread != NULL) {
			CloseHandle(hThread);
		}
	}
	return success;
}


BOOLEAN ModuleMappingClearModule(DWORD moduleIndex) {
	BOOLEAN success = TRUE;
	DWORD i, k;
	QWORD j;

	__try {
		i = moduleIndex;



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


		for (j = 0; j < g_dockerThreadIdLength; j++) {
			for (k = 0; k < PAGE_CALL_INFO_SIZE; k++) {
				if ((QWORD)g_mmm.Element[i].OldCode <= (QWORD)g_dockerThreadId[j].Docker->pageCallInfo[k].OldRetAddress &&
					(QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength >= (QWORD)g_dockerThreadId[j].Docker->pageCallInfo[k].OldRetAddress) {
					g_dockerThreadId[j].Docker->pageCallInfo[k].OldRetAddress = (QWORD)NULL;
					g_dockerThreadId[j].Docker->pageCallInfo[k].NewRetAddress = (QWORD)NULL;
				}
			}
		}

		VirtualFree(g_mmm.Element[i].CallInfo, 0, MEM_RELEASE);
		VirtualFree(g_mmm.Element[i].Instructions, 0, MEM_RELEASE);
		VirtualFree(g_mmm.Element[i].InstructionsMinimal, 0, MEM_RELEASE);
		VirtualFree(g_mmm.Element[i].NewCode, 0, MEM_RELEASE);
		VirtualFree(g_mmm.Element[i].NewData, 0, MEM_RELEASE);
		VirtualFree(g_mmm.Element[i].OldCodeMask, 0, MEM_RELEASE);
		if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {//we clear g_ppmp
			for (j = (QWORD)g_mmm.Element[i].OldCode; j < (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength; j++) {
				g_ppmp(j) = NULL;
			}
		}
		g_mmm.Element[i].OldCode = 0;
		g_mmm.Element[i].OldCodeLength = 0;
		g_mmm.Element[i] = g_mmm.Element[g_mmm.ElementLength - 1];
		if (((QWORD)g_mmm.Element[i].OldCode >> 36) == 0x7FF) {//we change g_ppmp
			for (j = (QWORD)g_mmm.Element[i].OldCode; j < (QWORD)g_mmm.Element[i].OldCode + (QWORD)g_mmm.Element[i].OldCodeLength; j++) {
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
	}
	__finally {

	}
	return success;
}

