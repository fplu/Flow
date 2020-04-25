#include "FlowMain.h"



BOOL ManageJmp(
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
		if (context == NULL) {//because jump could be call/ret and we need to get the context if so
			success = FALSE;
			__leave;
		}

		if ((dest = GetJmpDest(instructionRead->Opcode + instructionRead->EndOfPrefix, instructionRead->Rex, (const void*)docker->Rip, instructionRead->Length, context, instructionRead->AddressSize)) == (QWORD)NULL) {
			success = FALSE;
			__leave;
		}

//		if (!g_EnableRangeInstructionInstrumentation(dest, 1)) {

//		}
//		else {
			//STEP 1 : calc new instructions length

			//STEP 2 : check if there is enough space for the new instruction
			if (newInstructionLength + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
				success = FALSE;
				__leave;
			}

			//STEP 3 : write the new instruction

			//STEP 4 : give the new value of Rip
			(QWORD)docker->Rip = dest;
//		}
	}
	__finally {

	}
	return success;
}