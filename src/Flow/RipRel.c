#include "FlowMain.h"


BOOL ManageRipRelative(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
) {
	BOOL success = TRUE;
	DWORD newInstructionLength = 0;
	__try {
		//STEP 1 : calc new instructions length
		newInstructionLength += 24 + instructionRead->Length - 4;

		//STEP 2 : check if there is enough space for the new instruction
		if (newInstructionLength + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
			success = FALSE;
			__leave;
		}

		//STEP 3 : write the new instruction
		RewriteRipRelativeInstructionSaveZone(instructionRead, buffer + *bufferIndex, &docker->saveZone);
		*bufferIndex += 24 + instructionRead->Length - 4;

		//STEP 4 : give the new value of Rip
		(QWORD)docker->Rip += instructionRead->Length;
	}
	__finally {

	}
	return success;
}
