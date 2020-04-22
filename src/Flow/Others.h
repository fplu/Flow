#ifndef _RIP_OTHERS_H_
#define _RIP_OTHERS_H_

inline DWORD GetMappedOthersSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	return pInstruction->Length;
}

inline DWORD WriteMappedOthers(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* buffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	memCopy((char*)buffer, pInstruction->Opcode, pInstruction->Length);
	return GetMappedOthersSize(pPage, pInstruction, pIndex);
}


inline DWORD SingleSteppingOthersGetSize(INSTRUCTION_READ* pInstruction) {
	return pInstruction->Length;
}

inline DWORD SingleSteppingOthersWrite(INSTRUCTION_READ* pInstruction, OPCODE* buffer, THREAD_CONTEXT * pContext) {
	memCopy((char*)buffer, pInstruction->Opcode, pInstruction->Length);
	return SingleSteppingOthersGetSize(pInstruction);
}

inline QWORD SingleSteppingOthersGetRip(INSTRUCTION_READ* pInstruction, QWORD rip) {
	return rip + pInstruction->Length;
}

BOOL ManageBasicInstruction(
	_In_ DOCKER* pDocker,
	_In_opt_ THREAD_CONTEXT* pContext,
	_In_ INSTRUCTION_READ* pInstruction,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* pBufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_OTHERS_H_
