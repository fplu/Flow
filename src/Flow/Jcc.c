#include "FlowMain.h"



BOOL ManageJcc(
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
		//STEP 0 check argument
		if (context == NULL) {
			success = FALSE;
			__leave;
		}

		//STEP 1 : calc new instructions length

		//STEP 2 : check if there is enough space for the new instruction
		if (newInstructionLength + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
			success = FALSE;
			__leave;
		}

		//STEP 3 : write the new instruction

		//STEP 4 : give the new value of Rip
		if (IsJccOrLoopActive(instructionRead->Opcode + instructionRead->EndOfPrefix, (DWORD)context->EFlags, context->Rcx, instructionRead->AddressSize == 4)) {
			docker->Rip = (void*)GetJccOrLoopDest(instructionRead->Opcode + instructionRead->EndOfPrefix, (const void*)context->Rip, instructionRead->Length);
		}
		else {
			(QWORD)docker->Rip += instructionRead->Length;
		}
	}
	__finally {

	}
	return success;
}

