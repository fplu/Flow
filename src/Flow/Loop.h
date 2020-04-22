#ifndef _RIP_LOOP_H_
#define _RIP_LOOP_H_

inline DWORD GetMappedLoopSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		return GetMappedExitMappingSize(pPage, pInstruction, pIndex);
	}
	return pInstruction->Length + 10;
}


inline DWORD WriteMappedLoop(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* pBuffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		return WriteMappedExitMapping(pPage, pInstruction, pBuffer, (void(*)(void))(void*)pCallbackList->ppExit, tlsId, pIndex);
	}

	int i;
	for (i = 0; i < pInstruction->Length - 1; i++) {
		pBuffer[i] = pInstruction->Opcode[i];
	}
	pBuffer[i] = 0x05; i++;
	jmpRel32(pBuffer + i, pBuffer + i + 10); i += 5;
	jmpRel32(pBuffer + i, pInstruction->Dest); i += 5;
	return GetMappedLoopSize(pPage, pInstruction, pIndex);
}

BOOL ManageLoop(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);


#endif // !_RIP_LOOP_H_
