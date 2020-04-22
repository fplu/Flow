//ADD CALL (=> when call, callback)
//ADD RET (=> when call check stack to add bp then when bp is met callbacl and clear bp)

#include "FlowMain.h"

//extern void(*g_InstructionDefaultCallback)(EMULATOR_HANDLER*);
//void(*(*g_GetInstructionCallback)(INSTRUCTION_READ*))(EMULATOR_HANDLER*);


void CallBackGet(_In_opt_ DOCKER* docker, _In_opt_ INSTRUCTION_READ* instruction, _Out_writes_bytes_(*pCallbackTabLength*8) void ** callbackTab, _Inout_ DWORD * pCallbackTabLength) {
	DWORD index = 0;
	void* tmp;

	if ((tmp = g_InstructionDefaultCallback) && index < *pCallbackTabLength) {
		callbackTab[index] = g_InstructionDefaultCallback;
		index++;
		//__debugbreak();
	}

	if (docker && docker->instructionId != -1 && (tmp = docker->mappedInstructionMinimal[docker->instructionId].Callback) && index < *pCallbackTabLength) {
		callbackTab[index] = tmp;
		index++;
		//__debugbreak();
	}

	if (instruction && g_GetInstructionCallback && (tmp = g_GetInstructionCallback(instruction)) != NULL && index < *pCallbackTabLength) {
		callbackTab[index] = tmp;
		index++;
		//__debugbreak();
	}

	*pCallbackTabLength = index;
}

void CallBackExecute(_In_opt_ DOCKER* docker, _In_opt_ INSTRUCTION_READ* instruction) {
	void* callBack[16];
	DWORD callbackLength = _countof(callBack);
	DWORD i;

	CallBackGet(docker, NULL, callBack, &callbackLength);
	for (i = 0; i < callbackLength; i++) {
		((void(*)(EMULATOR_HANDLER*))callBack[i])((EMULATOR_HANDLER*)docker);
	}

}

void CallbackRun(DOCKER * docker) {

	jmpImm64(
		(QWORD)docker->callbackBaseAddress + (QWORD)docker->traceLoopTraceInstruction - (QWORD)docker->traceLoopStart,
		docker->mappedInstructionMinimal[docker->instructionId].NewAddress + 18
	);

	docker->context.Rip = (DWORD64)docker->Rip = (DWORD64)docker->mappedInstructionMinimal[docker->instructionId].OldAddress;

	CallBackExecute(docker, NULL);
	//HERE WE GO
	/*if (g_InstructionDefaultCallback) {
		g_InstructionDefaultCallback((EMULATOR_HANDLER*)docker);
	}
	else if (docker->mappedInstructionMinimal[docker->instructionId].Callback) {
		docker->mappedInstructionMinimal[docker->instructionId].Callback((EMULATOR_HANDLER*)docker);
	}*/

	docker->instructionId = -1;
}

