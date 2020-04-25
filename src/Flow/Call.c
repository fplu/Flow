#include "FlowMain.h"


BOOL ManageCall(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
) {
	BOOL success = TRUE;
	DWORD newInstructionLength = 0;
	QWORD dest;

	__try {
		//STEP 0 check argument
		if ((dest = GetCallDest(instructionRead->Opcode + instructionRead->EndOfPrefix, instructionRead->Rex, (const void*)docker->Rip, instructionRead->Length, context, instructionRead->AddressSize)) == (QWORD)NULL) {
			success = FALSE;
			__leave;
		}

		if (!g_EnableRangeInstructionInstrumentation(dest, 1)) {
			//STEP 1 : calc new instructions length
			newInstructionLength += 27;

			//STEP 2 : check if there is enough space for the new instruction
			if (newInstructionLength + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
				success = FALSE;
				__leave;
			}


			//STEP 3 : write the new instruction
			callUsingRet((buffer + *bufferIndex), dest, (buffer + *bufferIndex + 27));
			* bufferIndex += 27;

			(QWORD)docker->Rip = instructionRead->Opcode + instructionRead->Length;
		}
		else {
			//STEP 1 : calc new instructions length
			newInstructionLength += 13;

			//STEP 2 : check if there is enough space for the new instruction
			if (newInstructionLength + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
				success = FALSE;
				__leave;
			}

			//STEP 3 : write the new instruction
			pushimm64((buffer + *bufferIndex), (QWORD)((QWORD)docker->Rip + instructionRead->Length));
			*bufferIndex += 13;

			//STEP 4 : give the new value of Rip
			(QWORD)docker->Rip = dest;
		}
	}
	__finally {

	}
	return success;
}