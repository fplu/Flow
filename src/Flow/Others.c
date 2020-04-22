#include "FlowMain.h"

BOOL ManageBasicInstruction(
	_In_ DOCKER* pDocker,
	_In_opt_ THREAD_CONTEXT* pContext,
	_In_ INSTRUCTION_READ* pInstruction,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* pBufferIndex,
	_In_ DWORD bufferLength
) {
	BOOL success = TRUE;
	__try {
		if (SingleSteppingOthersGetSize(pInstruction) + *pBufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
			success = FALSE;
			__leave;
		}
		(*pBufferIndex) += SingleSteppingOthersWrite(pInstruction, buffer + *pBufferIndex, pContext);
		pDocker->Rip = (void*)SingleSteppingOthersGetRip(pInstruction, (QWORD)pDocker->Rip);
	}
	__finally {

	}
	return success;
}