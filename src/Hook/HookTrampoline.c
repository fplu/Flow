#include "Hook.h"

#define SIZEOpcodeHOOKTRAMPOLINE 14

extern DWORD tlsIndex;
extern OPCODE * g_OpcodeBase;

PListHook g_head = NULL;

BYTE pushRegisterOpcode[] = { 0x9C, 0x55, 0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x41, 0x50, 0x41, 0x51, 0x41, 0x52, 0x41, 0x53, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57 };
BYTE popRegisterOpcode[] = { 0x41, 0x5F, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x41, 0x5B, 0x41, 0x5A, 0x41, 0x59, 0x41, 0x58, 0x5F, 0x5E, 0x5A, 0x59, 0x5B, 0x58, 0x5D, 0x9D };

#define pushRegister(adresseOpcode, index) \
	for((index) = 0; (index) < sizeof(pushRegisterOpcode); (index)++) {\
		*(((BYTE*)(adresseOpcode)) + (index)) = pushRegisterOpcode[index];\
	}

#define popRegister(adresseOpcode, index) \
	for((index) = 0; (index) < sizeof(popRegisterOpcode); (index)++) {\
		*(((BYTE*)(adresseOpcode)) + (index)) = popRegisterOpcode[index];\
	}

BOOL initHook() {
	BOOL success = TRUE;

	__try {

		//the tls allocation
		if ((tlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
			TPRINTF_ERROR(_T("TlsAlloc fail\n"));
		}

		//location of trampoline
		if (!(g_OpcodeBase = VirtualAlloc(NULL, USN_PAGE_SIZE, MEM_COMMIT, PAGE_EXECUTE_READWRITE))) {
			TPRINTF_ERROR(_T("VirtualAlloc 1 fail\n"));
		}

	}
	__finally {

	}
	return success;
}


BOOL placeHookTrampoline(_In_ PVOID functionToReplace, _In_opt_ PVOID functionWhichReplace, _In_opt_ PVOID precallback, _In_opt_ PVOID postcallback) {
	BOOL success = TRUE;

	//size_t cumulatedLength = 0;
	//BYTE INSTRUCTION_READLength;
//	INSTRUCTION_READ INSTRUCTION_READ[SIZEOpcodeHOOKTRAMPOLINE];

	OPCODE * newINSTRUCTION = NULL;
	DWORD newINSTRUCTIONLenght = 0;

	DWORD oldProtect;

	DWORD i;
	DWORD indexOpcodeBase;
	DWORD startNextINSTRUCTION;

	__try {
		if (!functionToReplace) {
			SetLastError(ERROR_INVALID_PARAMETER);
			TPRINTF_ERROR(_T("Invalid Arg\n"));
		}

		if (!functionWhichReplace) {
			functionWhichReplace = __fonctionHookGeneric;
		}

		//count INSTRUCTION
/*		for (i = 0; cumulatedLength < SIZEOpcodeHOOKTRAMPOLINE; i++) {
			getINSTRUCTION((BYTE*)functionToReplace + cumulatedLength, INSTRUCTION_READ + i);
			cumulatedLength += INSTRUCTION_READ[i].Length;
		}
		INSTRUCTION_READLength = i;*/

		//get modified INSTRUCTION, it could fail if you try to hook too short function
		startNextINSTRUCTION = 14;
		if (!ModifyRipRelativeInstruction(functionToReplace, &startNextINSTRUCTION, &newINSTRUCTION, &newINSTRUCTIONLenght)) {
			TPRINTF_ERROR(_T("modifyRipRelativeINSTRUCTION fail\n"));
		}

		//temporarely change the protect for modification
		if (!VirtualProtect(functionToReplace, startNextINSTRUCTION, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			MSDN_FUNC_ERROR();
		}

		if (!pushListeHook(&g_head, 0)) {
			TPRINTF_ERROR(_T("pushListeHook fail\n"));
		}

		//place the trampoline
		indexOpcodeBase = g_head->indexStartHook;
		for (i = 0; i < newINSTRUCTIONLenght; i++) {
			g_OpcodeBase[indexOpcodeBase] = newINSTRUCTION[i];
			indexOpcodeBase++;
		}

		/*
		functionM : moved header of function hijacked
		functionH : function hijacked
		functionG : generic replacement function
		functionC : created function
		*/
		//jump from end of functionM to new start of functionH
		jmpUsingRet(g_OpcodeBase + indexOpcodeBase, (unsigned long long)functionToReplace + (unsigned long long)startNextINSTRUCTION);
		indexOpcodeBase += 14;

		//jump from start of functionH to start of functionC
		jmpUsingRet((BYTE*)functionToReplace, (unsigned long long)g_OpcodeBase + indexOpcodeBase);

		//creation of functionC
		//step 1 : push register
		pushRegister(g_OpcodeBase + indexOpcodeBase, i);
		indexOpcodeBase += i;
		//step 2 : save functionM start for future use in functionG
		subRsp100h(g_OpcodeBase + indexOpcodeBase);
		indexOpcodeBase += 7;
		callOneArg(g_OpcodeBase + indexOpcodeBase, (QWORD)g_OpcodeBase + g_head->indexStartHook, (QWORD)saveCallAddress);
		indexOpcodeBase += 37;
		callOneArg(g_OpcodeBase + indexOpcodeBase, (QWORD)precallback, (QWORD)savePrecallback);
		indexOpcodeBase += 37;
		callOneArg(g_OpcodeBase + indexOpcodeBase, (QWORD)postcallback, (QWORD)savePostcallback);
		indexOpcodeBase += 37;
		addRsp100h(g_OpcodeBase + indexOpcodeBase);
		indexOpcodeBase += 7;
		//step 3 : pop register
		popRegister(g_OpcodeBase + indexOpcodeBase, i);
		indexOpcodeBase += i;


		//step 4 : jump from end of functionC to start of functionG
		jmpUsingRet(g_OpcodeBase + indexOpcodeBase, (unsigned long long)functionWhichReplace);
		indexOpcodeBase += 14;


		g_head->sizeHook = indexOpcodeBase - g_head->indexStartHook;

		//replace the initial protect
		if (!VirtualProtect(functionToReplace, startNextINSTRUCTION, oldProtect, &oldProtect)) {
			TPRINTF_ERROR(_T("VirtualProtect 2 fail\n"));
		}

	}
	__finally {

	}
	return success;
}

