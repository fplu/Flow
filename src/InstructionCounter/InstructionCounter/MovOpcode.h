#ifndef _MOV_Opcode_H_
#define _MOV_Opcode_H_

#define BUFFER_OPCODE_SIZE 512
#define callback_SIZE 35

#define NO_LINKED_INSTRUCTION -1
#define INVALID_INSTRUCTION_ID -1

/**
 * Used internally.
 */
typedef enum FUNCTION_ANALYZATION_ERROR_e {
	NoError, NewFunctionTooBig, OriginalFunctionTooBig, AbnormalJmp, INSTRUCTIONsLengthTooLittle, WrongArgument, Data
}FUNCTION_ANALYZATION_ERROR;

/**
 * Used internally.
 */
typedef struct ANALYSED_INSTRUCTION_s {
	union {
		INSTRUCTION_READ;
		INSTRUCTION_READ Instruction;
	};
	DWORD LinkedInstruction;
	OPCODE * NewAddress;
}ANALYSED_INSTRUCTION;

/**
 * Used internally.
 */
typedef struct ANALYSED_FUNTION_s {
	ANALYSED_INSTRUCTION * Instructions;
	DWORD InstructionsLength;
	DWORD InstructionsMaxLength;
	DWORD OriginalAddressLength;
	DWORD NewAddressLength;
	FUNCTION_ANALYZATION_ERROR Error;
	OPCODE * OriginalAddress;
	OPCODE * NewAddress;

}ANALYSED_FUNCTION;

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOL ExtractEntireFunction(_Inout_ ANALYSED_FUNCTION * analyzedFunction, _In_ BOOLEAN useStack, _In_ BOOLEAN goAbove);


/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOL ModifyRipRelativeInstruction(_In_ OPCODE * start, _Inout_ DWORD * length, _Out_ OPCODE ** ppBuf, _Out_ DWORD * pBufLength);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOL GetNextRipValue(_In_ THREAD_CONTEXT * context, _In_ INSTRUCTION_READ * instructionRead, _In_ void * Rip, _Out_ void **  nextRip);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOL GetRipRelativeAddress(_In_ INSTRUCTION_READ * instruction, _In_ void * instructionAddress, _Out_ void ** ripRelativeAddress);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOL GetNewInstructionLength(_In_ INSTRUCTION_READ * oldInstruction, _Out_ BYTE * newInstructionLength);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOL CreateNewInstruction(_In_  INSTRUCTION_READ * oldInstruction, _In_ void * ripRelativeAddress, _In_ void * addressRet, _Out_ OPCODE * pBuf);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB void MovMyAssembly(OPCODE* startData, OPCODE * startCode, DWORD length, QWORD * entriesValue, DWORD entriesLength, OPCODE * newStartCode);


/**
 * Used internally. addressDest should be 28 + INSTRUCTION_READ->Length - 4 length at least
 */
inline void RewriteRipRelativeInstruction(INSTRUCTION_READ * instructionRead, OPCODE * addressDest) {
	static DWORD tlsIdRipRelative = 0;
	QWORD ripRelativeAddress;
	int i = 0;

	if (tlsIdRipRelative == 0) {
		tlsIdRipRelative = TlsAlloc();
	}

	ripRelativeAddress = (QWORD)((BYTE*)instructionRead->Opcode + *(int*)(instructionRead->Opcode + instructionRead->ModRMPos + 1) + instructionRead->Length);

	if ((instructionRead->Opcode[instructionRead->ModRMPos] & 0x38) == 0x38) {
		movRsiToTls(addressDest, tlsIdRipRelative); i += 9;

		movRsiImm64((addressDest + i), (QWORD)ripRelativeAddress); i += 10;
		memCopy(addressDest + i, instructionRead->Opcode, instructionRead->ModRMPos); i += instructionRead->ModRMPos;
		if (instructionRead->Rex.Signature == REX_SIGNATURE) {
			addressDest[17 + instructionRead->EndOfPrefix - 1] &= (~REX_B);
		}
		addressDest[i] = (instructionRead->Opcode[instructionRead->ModRMPos] & 0x38) + 0x06; i++;//modR/M Byte modifier pour utiliser rsi au lieu de rip + imm32
		memCopy(
			addressDest + i,
			instructionRead->Opcode + instructionRead->ModRMPos + 5,
			instructionRead->Length - instructionRead->ModRMPos - 5);
		i += instructionRead->Length - instructionRead->ModRMPos - 5;
		movTlsToRsi(addressDest + i, tlsIdRipRelative); i += 9;

	}
	else {
		movRdiToTls(addressDest, tlsIdRipRelative); i += 9;
		movRdiImm64((addressDest + i), (QWORD)ripRelativeAddress); i += 10;
		memCopy(addressDest + i, instructionRead->Opcode, instructionRead->ModRMPos); i += instructionRead->ModRMPos;
		if (instructionRead->Rex.Signature == REX_SIGNATURE) {
			addressDest[17 + instructionRead->EndOfPrefix - 1] &= (~REX_B);
		}
		addressDest[i] = (instructionRead->Opcode[instructionRead->ModRMPos] & 0x38) + 0x07; i++;//modR/M Byte modifier pour utiliser rdi au lieu de rip + imm32
		memCopy(
			addressDest + i,
			instructionRead->Opcode + instructionRead->ModRMPos + 5,
			instructionRead->Length - instructionRead->ModRMPos - 5);
		i += instructionRead->Length - instructionRead->ModRMPos - 5;
		movTlsToRdi(addressDest + i, tlsIdRipRelative); i += 9;
	}
}

/**
 * Used internally. addressDest should be 24 + INSTRUCTION_READ->Length - 4 length at least. saveZone should be at less than 2Go from addressDest
 */
inline void RewriteRipRelativeInstructionSaveZone(INSTRUCTION_READ * instructionRead, OPCODE * addressDest, QWORD * saveZone) {
	static DWORD tlsIdRipRelative = 0;
	QWORD ripRelativeAddress;
	int i = 0;

	if (tlsIdRipRelative == 0) {
		tlsIdRipRelative = TlsAlloc();
	}

	ripRelativeAddress = (QWORD)((BYTE*)instructionRead->Opcode + *(int*)(instructionRead->Opcode + instructionRead->ModRMPos + 1) + instructionRead->Length);

	if ((instructionRead->Opcode[instructionRead->ModRMPos] & 0x38) == 0x38) {
		movQwordRegister(addressDest, saveZone, RSI_ID); i += 7;
		movRsiImm64((addressDest + i), (QWORD)ripRelativeAddress); i += 10;
		memCopy(addressDest + i, instructionRead->Opcode, instructionRead->ModRMPos); i += instructionRead->ModRMPos;
		if (instructionRead->Rex.Signature == REX_SIGNATURE) {
			addressDest[17 + instructionRead->EndOfPrefix - 1] &= (~REX_B);
		}
		addressDest[i] = (instructionRead->Opcode[instructionRead->ModRMPos] & 0x38) + 0x06; i++;//modR/M Byte modifier pour utiliser rsi au lieu de rip + imm32
		memCopy(
			addressDest + i,
			instructionRead->Opcode + instructionRead->ModRMPos + 5,
			instructionRead->Length - instructionRead->ModRMPos - 5);
		i += instructionRead->Length - instructionRead->ModRMPos - 5;
		movRegisterQword(addressDest + i, saveZone, RSI_ID); i += 7;
	}
	else {
		movQwordRegister(addressDest, saveZone, RDI_ID); i += 7;
		movRdiImm64((addressDest + i), (QWORD)ripRelativeAddress); i += 10;
		memCopy(addressDest + i, instructionRead->Opcode, instructionRead->ModRMPos); i += instructionRead->ModRMPos;
		if (instructionRead->Rex.Signature == REX_SIGNATURE) {
			addressDest[17 + instructionRead->EndOfPrefix - 1] &= (~REX_B);
		}
		addressDest[i] = (instructionRead->Opcode[instructionRead->ModRMPos] & 0x38) + 0x07; i++;//modR/M Byte modifier pour utiliser rdi au lieu de rip + imm32
		memCopy(
			addressDest + i,
			instructionRead->Opcode + instructionRead->ModRMPos + 5,
			instructionRead->Length - instructionRead->ModRMPos - 5);
		i += instructionRead->Length - instructionRead->ModRMPos - 5;
		movRegisterQword(addressDest + i, saveZone, RDI_ID); i += 7;
	}
}

#endif // !_MOV_Opcode_H_
