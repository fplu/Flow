#include "FlowMain.h"


BOOL ManageRet(
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
		if (instructionRead->Opcode[instructionRead->EndOfPrefix] == IRET) {
			//TODO
			while (1) {
				Beep(1000, 1000);
			}
		}
		else if (instructionRead->Opcode[instructionRead->EndOfPrefix] == 0xC2) {
			newInstructionLength += 8;
		}
		else if (instructionRead->Opcode[instructionRead->EndOfPrefix] == 0xC3) {
			newInstructionLength += 8;
		}

		//STEP 2 : check if there is enough space for the new instruction
		if (newInstructionLength + *bufferIndex + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {
			success = FALSE;
			__leave;
		}

		//STEP 3 : write the new instruction
		if (instructionRead->Opcode[instructionRead->EndOfPrefix] == IRET) {
			//TODO
		}
		else if (instructionRead->Opcode[instructionRead->EndOfPrefix] == 0xC2) {
			buffer[*bufferIndex + 0] = 0x48;	// lea rsp [rsp + 8 + imm16]	// add rsp, 8 + imm16 without affecting rflag
			buffer[*bufferIndex + 1] = 0x8D;
			buffer[*bufferIndex + 2] = 0xA4;
			buffer[*bufferIndex + 3] = 0x24;
			*(DWORD*)(buffer + *bufferIndex + 4) = 0x8 + *(SHORT*)(instructionRead->Opcode + instructionRead->EndOfPrefix + 1);
			*bufferIndex += 8;
		}
		else if (instructionRead->Opcode[instructionRead->EndOfPrefix] == 0xC3) {
			buffer[*bufferIndex + 0] = 0x48;	// lea rsp [rsp + 8 + imm16]	// add rsp, 8 + imm16 without affecting rflag
			buffer[*bufferIndex + 1] = 0x8D;
			buffer[*bufferIndex + 2] = 0xA4;
			buffer[*bufferIndex + 3] = 0x24;
			*(DWORD*)(buffer + *bufferIndex + 4) = 0x8;
			*bufferIndex += 8;
		}

		//STEP 4 : give the new value of Rip
		docker->Rip = (void*)GetRetDest(instructionRead->Opcode + instructionRead->EndOfPrefix, (const void*)context->Rip, instructionRead->Length, (void*)context->Rsp, instructionRead->HasOperandSizePrefix);
	}
	__finally {

	}

	return success;
}