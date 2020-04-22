#include "MainInstructionCounter.h"

OPCODE g_InstructionRep[] = { 0x6C, 0x6D, 0xA4, 0xA5, 0x6E, 0x6F, 0xAC, 0xAD,  0xAA, 0xAB};
OPCODE g_InstructionRepe[] = {  0xA6, 0xA7, 0xAE, 0xAF };
OPCODE g_InstructionRepne[] = {  0xA6, 0xA7, 0xAE, 0xAF };

#define FIND_MATCH_RAX()\
	if(operandSize == 1) {\
		for (i = 0; ((BYTE*)memoryAccess->BaseAddress)[i] != (BYTE)context->Rax; i++);\
	}\
	else if(operandSize == 2) {\
		for (i = 0; ((WORD*)memoryAccess->BaseAddress)[i] != (WORD)context->Rax; i++);\
	}\
	else if(operandSize == 4) {\
		for (i = 0; ((DWORD*)memoryAccess->BaseAddress)[i] != (DWORD)context->Rax; i++);\
	}\
	else if(operandSize == 8) {\
		for (i = 0; ((QWORD*)memoryAccess->BaseAddress)[i] != (QWORD)context->Rax; i++);\
	}

#define FIND_UNMATCH_RAX()\
	if(operandSize == 1) {\
		for (i = 0; ((BYTE*)memoryAccess->BaseAddress)[i] == (BYTE)context->Rax; i++);\
	}\
	else if(operandSize == 2) {\
		for (i = 0; ((WORD*)memoryAccess->BaseAddress)[i] == (WORD)context->Rax; i++);\
	}\
	else if(operandSize == 4) {\
		for (i = 0; ((DWORD*)memoryAccess->BaseAddress)[i] == (DWORD)context->Rax; i++);\
	}\
	else if(operandSize == 8) {\
		for (i = 0; ((QWORD*)memoryAccess->BaseAddress)[i] == (QWORD)context->Rax; i++);\
	}

#define FIND_UNMATCH_RSI_RDI()\
	if(operandSize == 1) {\
		for (i = 0; ((BYTE*)context->Rdi)[i] == ((BYTE*)context->Rsi)[i]; i++);\
	}\
	else if(operandSize == 2) {\
		for (i = 0; ((WORD*)context->Rdi)[i] == ((WORD*)context->Rsi)[i]; i++);\
	}\
	else if(operandSize == 4) {\
		for (i = 0; ((DWORD*)context->Rdi)[i] == ((DWORD*)context->Rsi)[i]; i++);\
	}\
	else if(operandSize == 8) {\
		for (i = 0; ((QWORD*)context->Rdi)[i] == ((QWORD*)context->Rsi)[i]; i++);\
	}

#define FIND_MATCH_RSI_RDI()\
	if(operandSize == 1) {\
		for (i = 0; ((BYTE*)context->Rdi)[i] != ((BYTE*)context->Rsi)[i]; i++);\
	}\
	else if(operandSize == 2) {\
		for (i = 0; ((WORD*)context->Rdi)[i] != ((WORD*)context->Rsi)[i]; i++);\
	}\
	else if(operandSize == 4) {\
		for (i = 0; ((DWORD*)context->Rdi)[i] != ((DWORD*)context->Rsi)[i]; i++);\
	}\
	else if(operandSize == 8) {\
		for (i = 0; ((QWORD*)context->Rdi)[i] != ((QWORD*)context->Rsi)[i]; i++);\
	}

/*
	it does not consider the following element (they are performed as MOV without segment override)
	LEA, NOP, FS AND GS
*/
BOOL GetInstructionMemoryAccess(
	_In_ MEMORY_ACCESS_INFORMATION memoryAccessInformation,
	_In_ const OPCODE * opcodeAfterPrefix,
	_In_ MODRM modrm,
	_In_ REX rex,
	_In_ const OPCODE * opcodeAfterModRm,
	_In_ const THREAD_CONTEXT * context,
	_In_ BYTE instructionLength,
	_In_ BYTE addressSize,
	_In_ BOOL hasRepPrefix,
	_In_ BOOL hasRepnePrefix,
	_Out_ MEMORY_ACCESS * memoryAccess,
	_Out_ BOOLEAN * isThereMemoryAccess
) {
	BOOL success = TRUE;
	DWORD i;
	BYTE operandSize;
	BOOL isInstructionRep = FALSE;
	BOOL isInstructionRepe = FALSE;
	BOOL isInstructionRepne = FALSE;

	__try {
		if (memoryAccess == NULL) {
			success = FALSE;
			__leave;
		}

		memoryAccess->IsThereBaseAddressBis = FALSE;

		if (memoryAccessInformation.Operand == OPERAND_NO_ACCESS) {
			*isThereMemoryAccess = FALSE;
			memoryAccess->BaseAddress = NULL;
			memoryAccess->Size = 0;
			__leave;
		}

		if (memoryAccessInformation.Operand == OPERAND_MODRM_ACCESS) {
			memoryAccess->BaseAddress = GetModRMAddress(modrm, rex, opcodeAfterModRm, context, instructionLength, addressSize, isThereMemoryAccess);
			memoryAccess->Size = memoryAccessInformation.Size;
			__leave;
		}

		operandSize = (BYTE)memoryAccessInformation.Size;

		*isThereMemoryAccess = TRUE;
		if (hasRepPrefix) {
			for (i = 0; i < _countof(g_InstructionRep); i++) {
				if (opcodeAfterPrefix[0] == g_InstructionRep[i]) {
					isInstructionRep = TRUE;
				}
			}
		}

		if (hasRepPrefix && !isInstructionRep) {
			for (i = 0; i < _countof(g_InstructionRep); i++) {
				if (opcodeAfterPrefix[0] == g_InstructionRep[i]) {
					isInstructionRepe = TRUE;
				}
			}
		}

		if (hasRepnePrefix) {
			for (i = 0; i < _countof(g_InstructionRep); i++) {
				if (opcodeAfterPrefix[0] == g_InstructionRep[i]) {
					isInstructionRepne = TRUE;
				}
			}
		}

		else if (memoryAccessInformation.Operand == OPERAND_RDI_ACCESS) {
			memoryAccess->BaseAddress = (void*)context->Rdi;
			if (isInstructionRep) {
				memoryAccess->Size = (DWORD)(context->Rcx * operandSize);
			}
			else if (isInstructionRepe) {
				FIND_UNMATCH_RAX();
				memoryAccess->Size = i * operandSize;
			}
			else if (isInstructionRepne) {
				FIND_MATCH_RAX();
				memoryAccess->Size = i * operandSize;
			}
			else {
				memoryAccess->Size = operandSize;
			}
		}
		else if (memoryAccessInformation.Operand == OPERAND_RSI_ACCESS) {
			memoryAccess->BaseAddress = (void*)context->Rsi;
			if (isInstructionRep) {
				memoryAccess->Size = (DWORD)(context->Rcx * operandSize);
			}
			else if (isInstructionRepe) {
				FIND_UNMATCH_RAX();
				memoryAccess->Size = i * operandSize;
			}
			else if (isInstructionRepne) {
				FIND_MATCH_RAX();
				memoryAccess->Size = i * operandSize;
			}
			else {
				memoryAccess->Size = operandSize;
			}
		}
		else if (memoryAccessInformation.Operand == OPERAND_RDI_AND_RSI_ACCESS) {
			memoryAccess->IsThereBaseAddressBis = TRUE;
			memoryAccess->BaseAddress = (void*)context->Rdi;
			memoryAccess->BaseAddressBis = (void*)context->Rsi;
			if (isInstructionRep) {
				memoryAccess->Size = (DWORD)(context->Rcx * operandSize);
			}
			else if (isInstructionRepe) {
				FIND_UNMATCH_RSI_RDI();
				memoryAccess->Size = i * operandSize;
			}
			else if (isInstructionRepne) {
				FIND_MATCH_RSI_RDI();
				memoryAccess->Size = i * operandSize;
			}
			else {
				memoryAccess->Size = operandSize;
			}
		}

	}
	__finally {

	}


	return success;
}