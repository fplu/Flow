#ifndef _RIP_SYSCALL_H_
#define _RIP_SYSCALL_H_

#define SYSCALL_ADDING_SIZE 24


inline DWORD GetMappedSyscallSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
//	return GetMappedExitMappingSize(pPage, pInstruction, pIndex);
	if (g_SyscallDefaultCallback != NULL) {
		return GetMappedExitMappingSize(pPage, pInstruction, pIndex);
	}
	return GetMappedExitMappingSize(pPage, pInstruction, pIndex) + SYSCALL_ADDING_SIZE + pInstruction->Length;
}


inline DWORD WriteMappedSyscall(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* pBuffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	//	return WriteMappedExitMapping(pPage, pInstruction, pBuffer, exit, tlsId, pIndex);
	if (g_SyscallDefaultCallback != NULL) {
		return WriteMappedExitMapping(pPage, pInstruction, pBuffer, (void(*)(void))(void*)pCallbackList->ppSyscall, tlsId, pIndex);
	}

	movQwordToTls(pBuffer, g_TlsIdStoreR2, pBuffer + SYSCALL_ADDING_SIZE + 18);
	memCopy(pBuffer + SYSCALL_ADDING_SIZE + 18, pInstruction->Opcode, pInstruction->Length);
	return WriteMappedExitMapping(pPage, pInstruction, pBuffer + SYSCALL_ADDING_SIZE, (void(*)(void))(void*)pCallbackList->ppSyscall, tlsId, pIndex) + SYSCALL_ADDING_SIZE + pInstruction->Length;
}

#undef SYSCALL_ADDING_SIZE

BOOL ManageSyscall(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_SYSCALL_H_
