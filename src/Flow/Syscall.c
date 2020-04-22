#include "FlowMain.h"

BOOL ManageSyscall(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
) {
	BOOL success = TRUE;
	DWORD newInstructionLength = 0;
	DWORD i;
	__try {

		if (context == NULL) {
			success = FALSE;
			__leave;
		}

		if (g_SyscallDefaultCallback) {
			success = g_SyscallDefaultCallback(context, (QWORD*)&docker->Rip, (EMULATOR_HANDLER*)docker, buffer, bufferIndex);
			__leave;
		}

		for (i = 0; i < g_SyscallCallbackLength; i++) {
			if (context->Rax == g_SyscallCallback[i].id) {
				success = g_SyscallCallback[i].address(context, (QWORD*)&docker->Rip, (EMULATOR_HANDLER*)docker, buffer, bufferIndex);
				__leave;
			}
		}

		CALLBACK_WRITE_SYSCALL();

		//STEP 4 : give the new value of Rip
	}
	__finally {
		if (success) {
			(QWORD)docker->Rip += instructionRead->Length;
		}
	}
	return success;
}
