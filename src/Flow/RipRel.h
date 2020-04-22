#ifndef _RIP_RIPREL_H_
#define _RIP_RIPREL_H_

inline DWORD GetMappedRipRelSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {

	if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0x8A ||
		pInstruction->Opcode[pInstruction->EndOfPrefix] == 0x8B) {
		DWORD i = 0;
		MODRM modrm;
		REX rex = { 0 };
		modrm = *(MODRM*)(pInstruction->Opcode+pInstruction->ModRMPos);
		modrm.Rm = modrm.Reg;
		modrm.Mod = 1;
		if (pInstruction->Rex.Signature == REX_SIGNATURE) {
			rex = pInstruction->Rex;
			rex.B = rex.R;
		}
		
		/*if (modrm.Rm == 4 || modrm.Mod != 0 || pInstruction->ModRMPos == 255 || rex.Signature == REX_SIGNATURE) {
			return 28 + pInstruction->Length - 4;
		}*/
		
		i += 10 + pInstruction->ModRMPos + 2 + pInstruction->Length - pInstruction->ModRMPos - 5;
		if (modrm.Rm == 4) {
			i++;
		}
		return i;
	}
	return 28 + pInstruction->Length - 4;
}

inline DWORD WriteMappedRipRel(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* buffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {


	if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0x8A ||
		pInstruction->Opcode[pInstruction->EndOfPrefix] == 0x8B) {
		//__debugbreak();
		QWORD ripRelativeAddress = (QWORD)((BYTE*)pInstruction->Opcode + *(int*)(pInstruction->Opcode + pInstruction->ModRMPos + 1) + pInstruction->Length);
		DWORD i = 0;
		REX rex = { 0 };
		MODRM modrm;
		SIB sib;
		modrm = *(MODRM*)(pInstruction->Opcode + pInstruction->ModRMPos);
		modrm.Rm = modrm.Reg;
		modrm.Mod = 1;
		if (pInstruction->Rex.Signature == REX_SIGNATURE) {
			rex = pInstruction->Rex;
			rex.B = rex.R;
		}
		
		/*if (modrm.Rm == 4 || modrm.Mod != 0 || pInstruction->ModRMPos == 255 || rex.Signature == REX_SIGNATURE) {
			RewriteRipRelativeInstruction(&(pInstruction->Instruction), buffer);
			return 28 + pInstruction->Length - 4;
		}*/
		
		if (rex.Signature == REX_SIGNATURE && rex.R) {
			movRegExtendImm64(buffer, ((MODRM*)(pInstruction->Opcode + pInstruction->ModRMPos))->Reg, ripRelativeAddress); i += 10;
		}
		else {
			movRegImm64(buffer, ((MODRM*)(pInstruction->Opcode + pInstruction->ModRMPos))->Reg, ripRelativeAddress); i += 10;
		}
		memCopy(buffer + i, pInstruction->Opcode, pInstruction->ModRMPos); i += pInstruction->ModRMPos;
		if (rex.Signature == REX_SIGNATURE) {
			buffer[10 + pInstruction->EndOfPrefix - 1] = rex.EntireByte;
		}
		buffer[i] = *(BYTE*)&modrm; i++;
		if (modrm.Rm == 4) {
			sib.Scale = 0;
			sib.Base = 4;
			sib.Index = 4;
			buffer[i] = *(BYTE*)&sib; i++;
		}
		buffer[i] = 0; i++;
		memCopy(buffer + i, pInstruction->Opcode + pInstruction->ModRMPos + 5, pInstruction->Length - pInstruction->ModRMPos - 5);
		i += pInstruction->Length - pInstruction->ModRMPos - 5;

	}
	else {
		RewriteRipRelativeInstruction(&(pInstruction->Instruction), buffer);
	}
	return GetMappedRipRelSize(pPage, pInstruction, pIndex);
}

inline DWORD SingleSteppingRipRelGetSize(INSTRUCTION_READ* pInstruction) {
	return 28 + pInstruction->Length - 4;
}

inline DWORD SingleSteppingRipRelWrite(INSTRUCTION_READ* pInstruction, OPCODE* buffer, THREAD_CONTEXT* pContext) {
	RewriteRipRelativeInstruction(pInstruction, buffer);
	return SingleSteppingRipRelGetSize(pInstruction);
}

inline QWORD SingleSteppingRipRelGetRip(INSTRUCTION_READ* pInstruction, QWORD rip) {
	return rip + pInstruction->Length;
}

BOOL ManageRipRelative(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_RIPREL_H_
