#include "MainInstructionCounter.h"

INSTRUCTION_READ NopInstruction() {
	INSTRUCTION_READ res;
	res.IsRipRelative = FALSE;
	res.IsRet = FALSE;
	res.IsJcc = FALSE;
	res.IsLoop = FALSE;
	res.IsCall = FALSE;
	res.IsJmp = FALSE;
	res.EndOfPrefix = 0;
	res.ModRMPos = 255;
	res.Length = 1;
	res.Rex.EntireByte = 0x0;
	res.HasOperandSizePrefix = FALSE;
	res.AddressSize = 8; // 4 or 8 (8 is default value)
	res.HasLockPrefix = FALSE;
	res.HasRepePrefix = FALSE;
	res.HasRepnePrefix = FALSE;
	//res.Precallback = NULL;
	//res.Postcallback = NULL;
	res.Text = "NOP";
	res.MemoryAccess.Size = 0;
	res.MemoryAccess.Operand = OPERAND_NO_ACCESS;
	res.Opcode = (OPCODE[]){ NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP };
	return res;
}

BOOL GetMemoryAccessFromInstructionRead(
	_In_ const INSTRUCTION_READ * pInstructionRead,
	_In_ const THREAD_CONTEXT * pContext,
	_Out_ MEMORY_ACCESS * pMemoryAccess,
	_Out_ BOOLEAN * pIsThereMemoryAccess
) {
	DWORD i;
	REX rex;

	for (i = 0; i < pInstructionRead->EndOfPrefix; i++) {
		if (pInstructionRead->Opcode[i] == SEGMENT_OVERRIDE_GS_PREFIX ||
			pInstructionRead->Opcode[i] == SEGMENT_OVERRIDE_FS_PREFIX) {
			*pIsThereMemoryAccess = FALSE;
			return TRUE;
		}
	}

	
	/*if (*(DWORD*)instructionRead->Opcode == *(DWORD*)(BYTE[]) { 0xf3, 0xa4, 0x49, 0x8b }) {
		DebugBreak();
	}*/
	//if (*(QWORD*)instructionRead->Text == *(QWORD*)"VPCMPEQW ymm1, y") {
		//DebugBreak();
	//}

	rex = pInstructionRead->Rex;
	if (pInstructionRead->Opcode[pInstructionRead->EndOfPrefix] == 0xC4) {
		rex.Signature = REX_SIGNATURE;
		rex.B = !((pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 1] & 0x20) == 0x20);
		rex.X = !((pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 1] & 0x40) == 0x40);
		rex.R = !((pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 1] & 0x80) == 0x80);
		rex.W = ((pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 2] & 0x80) == 0x80);
	}
	else if (pInstructionRead->Opcode[pInstructionRead->EndOfPrefix] == 0xC5) {
		rex.Signature = REX_SIGNATURE;
		rex.B = 0;
		rex.X = 0;
		rex.R = !((pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 1] & 0x80) == 0x80);
		rex.W = 0;
	}

	return GetInstructionMemoryAccess(
		pInstructionRead->MemoryAccess,
		pInstructionRead->Opcode + pInstructionRead->EndOfPrefix,
		*(MODRM*)(pInstructionRead->Opcode + pInstructionRead->ModRMPos),
		rex,
		pInstructionRead->Opcode + pInstructionRead->ModRMPos + 1,
		pContext,
		pInstructionRead->Length,
		pInstructionRead->AddressSize,
		pInstructionRead->HasRepePrefix,
		pInstructionRead->HasRepnePrefix,
		pMemoryAccess,
		pIsThereMemoryAccess);
}

BOOL UseGs(_In_ INSTRUCTION_READ * pInstructionRead) {
	DWORD i;
	if (pInstructionRead->Opcode[pInstructionRead->EndOfPrefix] == READ_SEG &&
		((MODRM*)&pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 1])->Mod == GS_ID) {//MOV r/m, Sreg
		return TRUE;
	}
	if (pInstructionRead->Opcode[pInstructionRead->EndOfPrefix] == WRITE_SEG &&
		((MODRM*)&pInstructionRead->Opcode[pInstructionRead->EndOfPrefix + 1])->Mod == GS_ID) {//MOV Sreg, r/m
		return TRUE;
	}
	for (i = 0; i < pInstructionRead->EndOfPrefix; i++) { //gs segment override
		if (pInstructionRead->Opcode[i] == SEGMENT_OVERRIDE_GS_PREFIX) {
			return TRUE;
		}
	}
	return FALSE;
}


_Success_(return) BOOL GetModRMRegFromInstructionRead(_In_ INSTRUCTION_READ* pInstructionRead, _In_ const THREAD_CONTEXT* pContext, _Out_ QWORD* value) {

	if (pInstructionRead->ModRMPos == 255) {
		return FALSE;
	}

	*value = (QWORD)getRegisterValue(((MODRM*)(
		pInstructionRead->Opcode + pInstructionRead->ModRMPos))->Reg,
		pInstructionRead->Rex,
		8,
		pInstructionRead->Rex.R,
		pContext
	);

	return TRUE;
}

//return the size of the instruction "to be/which was" written to the buffer
DWORD movRdiModRM(_In_ INSTRUCTION_READ * pInstruction, _In_opt_ OPCODE * buffer) {
	MODRM modrm;
	BYTE modrmSize = GetModRmSize(pInstruction->Opcode + pInstruction->ModRMPos);
	DWORD i = 0;
	REX rex;
	QWORD dest;

	rex.EntireByte = 0x48;
	rex.B = pInstruction->Rex.B;
	rex.X = pInstruction->Rex.X;
	modrm = *(MODRM*)(pInstruction->Opcode + pInstruction->ModRMPos);
	modrm.Reg = RDI_ID;
	if (buffer != NULL) {
		if (IS_MODRM_RIP_RELATIVE(pInstruction->Opcode[pInstruction->ModRMPos])) {
			dest = (QWORD)((BYTE*)pInstruction->Opcode + *(int*)(pInstruction->Opcode + pInstruction->ModRMPos + 1) + pInstruction->Length);
			movRdiImm64(buffer + i, dest); i += 10;
			buffer[i] = 0x48; i++;//mov rdi, [rdi]
			buffer[i] = 0x8B; i++;
			buffer[i] = 0x3F; i++;
		}
		else {
			buffer[i] = rex.EntireByte; i++;//mov rdi, [???]
			buffer[i] = 0x8B; i++;
			buffer[i] = *(OPCODE*)&modrm; i++;
			memCopy(buffer + i, pInstruction->Opcode + pInstruction->ModRMPos + 1, modrmSize); i += modrmSize;
		}
	}
	else {
		if (IS_MODRM_RIP_RELATIVE(pInstruction->Opcode[pInstruction->ModRMPos])) {
			i += 13;
		}
		else {
			i += 3 + modrmSize;
		}
	}

	return i;
}

