#ifndef _RIP_RET_H_
#define _RIP_RET_H_

#define RET_ADDING_SIZE 12

inline QWORD PopShadowStack() {
	//mov [tls0], rax
	//mov rax, [tlsPage]
	//mov [rax + 00h], rcx
	//mov [rax + 08h], rsp
	//lea rsp, [rax + 18h]
	//pushfq

	//lea rsp, [rel jmpHere_1_8]
	//mov [ReturnAddress], rsp; (store at rax + 1000h - 18h)

	//Mov rcx, [tlsShadowStackCounter]; (store at rax + 1000h - 08h)
	
	//mov rsp, rcx
	//dec rsp
	//movzx rsp, spl
	//mov [tlsShadowStackCounter], rsp; (store at rax + 1000h - 08h)

	//lea rcx, [rcx * 2 + rcx]
	//lea rcx, [rcx * 8 + tlsShadowStack]; (store at rax + 1000h - 10h)
	//mov rax, [rax + 08h];
	//cmp [rcx], rax
	//jne the_bad_way_8

		//mov rax, [rcx + 10h]
		//test rax, rax
		//jnz avoid_callback_8
			//jmp rel callback
		//avoid_callback_8:

		//mov rax, [rcx + 8h]
		//mov rcx, [tls2]
		//mov ecx, ecx
		//jmp end_of_the_good_way

	//the_bad_way_8:
		//lea rax, [rel startgocallback_ret]
		//xor ecx, ecx
	//end_of_the_good_way:

	//mov [tls3], rax

	//mov rax, [tlsPage]
	//lea rsp, [rax + 10h]
	//popfq
	//mov rsp, [rax + 08h]
	//lea rsp, [rsp + rcx]
	//mov rcx, [rax + 00h]
	//mov [tls0], rax

	//jmp [tls3]
}

inline DWORD GetMappedRetSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	
	return (GetMappedExitMappingSize(pPage, pInstruction, pIndex) + RET_ADDING_SIZE);
}


inline DWORD WriteMappedRet(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* pBuffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	DWORD deltaRsp = 8;
	if (pInstruction->Opcode[pInstruction->EndOfPrefix] == 0xC2) {
		deltaRsp += *(WORD*)(pInstruction->Opcode + pInstruction->EndOfPrefix + 1);
	}
	movDwordToTls(pBuffer, g_TlsIdStoreR2, deltaRsp);
	return (WriteMappedExitMapping(pPage, pInstruction, pBuffer + 12, (void(*)(void))(void*)pCallbackList->ppRet2, tlsId, pIndex) + RET_ADDING_SIZE);
}

#undef RET_ADDING_SIZE

BOOL ManageRet(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_RET_H_
