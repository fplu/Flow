#ifndef _RIP_JCC_H_
#define _RIP_JCC_H_


inline DWORD GetMappedJccSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		return GetMappedExitMappingSize(pPage, pInstruction, pIndex);
	}
	//jcx instruction
	else if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0xE3) {
		return pInstruction->Length + 10;
	}
	//jcc rel32
	else if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0x0F) {
		return 6;
	}
	//jcc rel8 except jcx
	else {
		return 6;
	}
}

inline DWORD WriteMappedJcc(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* pBuffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		return WriteMappedExitMapping(pPage, pInstruction, pBuffer, (void(*)(void))(void*)pCallbackList->ppExit, tlsId, pIndex);
	}
	//jcx
	else if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0xE3) {
		int i;
		for (i = 0; i < pInstruction->Length - 1; i++) {
			pBuffer[i] = pInstruction->Opcode[i];
		}
		pBuffer[i] = 0x05; i++;
		jmpRel32(pBuffer + i, pBuffer + i + 10); i += 5;
		jmpRel32(pBuffer + i, pInstruction->Dest); i += 5;
	}
	//jcc rel32
	else if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0x0F) {
		jccRel32(pBuffer, pInstruction->Dest, pInstruction->Opcode[pInstruction->EndOfPrefix + 1]);
	}
	//jcc rel8 except jcx
	else {
		jccRel32(pBuffer, pInstruction->Dest, pInstruction->Opcode[pInstruction->EndOfPrefix] + 0x10);
	}

	return GetMappedJccSize(pPage, pInstruction, pIndex);
}

BOOL ManageJcc(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_JCC_H_
