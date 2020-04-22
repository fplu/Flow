#ifndef _RIP_CALL_H_
#define _RIP_CALL_H_

#define FILL_CALL_INFO_BUFFER(newRetAddress) \
	pPage->CallInfo[pPage->CallInfoLength].OldRetAddress = (QWORD)(pInstruction->Opcode + pInstruction->Length);\
	pPage->CallInfo[pPage->CallInfoLength].NewRetAddress = (QWORD)(newRetAddress);\
	pPage->CallInfo[pPage->CallInfoLength].RetCallback = NULL;\
	pPage->CallInfoLength++;

inline QWORD PushShadowStack() {
//instruction ;not in case 2
	//mov [tls0], rax
	//mov rax, [tlsPage]
	//mov [CallIndex], imm32;(store at rax + 1000h - 20h)
	//;case 1 and 4 mov [jmpDest], destLow
	//;case 1 and 4 mov [jmpDest + 4], destHigh
	//mov rax, [tls0]


//param CallDest (it go there), CallIndex (it push the element at this index in shadow stack)
//function ;not in case 2, only in case 3 success
	//mov [tls0], rax
	//mov rax, [tlsPage]
	//mov [rax + 00h], rcx
	//mov [rax + 08h], rdx

	//mov rdx, [JmpDest];(store at rax + 1000h - 28h)
	//mov [tls2], rdx

	//mov rdx, [CallIndex];(store at rax + 1000h - 20h)
	//lea rdx, [rdx * 2 + rdx]
	//lea rdx, [rax + rdx * 8]

	//mov rcx, [tlsShadowStackCounter]; (store at rax + 1000h - 08h)
	//lea rcx, [rcx + 1]
	//movzx rcx, cl
	//mov [tlsShadowStackCounter], rcx; (store at rax + 1000h - 08h)
	//lea rcx, [rcx * 2 + rcx]
	//lea rcx, [rcx * 8 + tlsShadowStack]; (store at rax + 1000h - 10h)
	//mov rax, [rel moduleCallTab];
	//mov rax, [rdx]
	//mov [rcx], rax; old return value
	//mov rax, [rdx + 08h]
	//mov [rcx + 08h], rax; new return value
	//mov rax, [rdx + 10h]
	//mov [rcx + 10h], rax; optional callback

	//mov rax, [tlsPage]
	//mov rcx, [rax + 00h]
	//mov rdx, [rax + 08h]
	//mov [tls0], rax

	//jmp [tls2]
}

inline QWORD MappedCallModRmRipRel(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction) {
	if (IS_MODRM_RIP_RELATIVE(pInstruction->Opcode[pInstruction->ModRMPos])) {
		QWORD* pDest;

		if (pInstruction->AddressSize != 8) {
			return (QWORD)NULL;
		}

		pDest = (QWORD*)((BYTE*)pInstruction->Opcode + *(int*)(pInstruction->Opcode + pInstruction->ModRMPos + 1) + pInstruction->Length);

		if (IsAddressInDataPage(pPage, (QWORD)(void*)pDest)) {
			return ModuleMappingGetNewAddressFromOldAddress(NULL, *pDest, FALSE);
		}
	}
	return (QWORD)NULL;
}



inline DWORD GetMappedCallSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		/*if (MappedCallModRmRipRel(pPage, pInstruction) != NULL) {
			//pPage->NewDataLength++;
			return 54;
		}*/

		if (pInstruction->ModRMPos == 255) {
			return  GetMappedExitMappingSize(pPage, pInstruction, pIndex);
		}

		DWORD i = 0;

		i += 9;
		i += movRdiModRM(&(pInstruction->Instruction), NULL);
		i += 66;

		return GetMappedExitMappingSize(pPage, pInstruction, pIndex) + i;
	}
	//	return GetMappedExitMappingSize(pInstruction, pIndex);
	
	return 54;
	return 18;
}

inline DWORD WriteMappedCall(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* buffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	DWORD i = 0;


	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
	//	QWORD dest;
		/*if ((dest = MappedCallModRmRipRel(pPage, pInstruction)) != NULL) {
			//pPage->NewData[pPage->NewDataLength] = dest;

			movRaxToTls(buffer + i, g_TlsIdStoreR1); i += 9;
			movTlsToRax(buffer + i, g_TlsIdStorePage); i += 9;
			i += movAtRaxRelImm32(buffer + i, 0x1000 - 0x20, pPage->CallInfoLength);
			i += movAtRaxRelImm64(buffer + i, 0x1000 - 0x28, dest);
			jmpAtAddress(buffer + i, pCallbackList->ppCall2); i += 6;

//			pushimm64(buffer, pInstruction->Opcode + pInstruction->Length);
//			jmpAtAddress(buffer + 13, pPage->NewData + pPage->NewDataLength);
			FILL_CALL_INFO_BUFFER(buffer + 54);
			return GetMappedCallSize(pPage, pInstruction, pIndex);;
		}*/

		if (pInstruction->ModRMPos == 255) {
			FILL_CALL_INFO_BUFFER(buffer + 18);
			return  WriteMappedExitMapping(pPage, pInstruction, buffer, (void(*)(void))(void*)pCallbackList->ppExit, tlsId, pIndex);
		}

		i += movRdiToTls(buffer + i, g_TlsIdStoreR1);
		i += movRdiModRM(&(pInstruction->Instruction), buffer + i);
		movRdiToTls(buffer + i, g_TlsIdStoreR2); i += 9;
//		movRdiImm64(buffer + i, pInstruction->Opcode + pInstruction->Length); i += 10;
//		movRdiToTls(buffer + i, g_TlsIdStoreR2); i += 9;
		movTlsToRdi(buffer + i, g_TlsIdStoreR1); i += 9;

		movRaxToTls(buffer + i, g_TlsIdStoreR1); i += 9;
		movTlsToRax(buffer + i, g_TlsIdStorePage); i += 9;
		i += (DWORD)movAtRaxRelImm32(buffer + i, 0x1000 - 0x20, pPage->CallInfoLength);
		i += (DWORD)movAtRaxRelImm64(buffer + i, 0x1000 - 0x28, pInstruction->Dest);

		///__debugbreak();
		FILL_CALL_INFO_BUFFER(buffer + i + 18);
		return WriteMappedExitMapping(pPage, pInstruction, buffer + i, (void(*)(void))(void*)pCallbackList->ppCall, tlsId, pIndex) + i;
	}

	movRaxToTls(buffer + i, g_TlsIdStoreR1); i += 9;
	movTlsToRax(buffer + i, g_TlsIdStorePage); i += 9;
	i += (DWORD)movAtRaxRelImm32(buffer + i, 0x1000 - 0x20, pPage->CallInfoLength);
	i += (DWORD)movAtRaxRelImm64(buffer + i, 0x1000 - 0x28, pInstruction->Dest);
	jmpAtAddress(buffer + i, pCallbackList->ppCall2); i += 6;
	

	//pushimm64(buffer + i, pInstruction->Opcode + pInstruction->Length); i += 13;
	//jmpRel32(buffer + i, pInstruction->Dest); i += 5;

	FILL_CALL_INFO_BUFFER(buffer + 54);
	return GetMappedCallSize(pPage, pInstruction, pIndex);
}


BOOL ManageCall(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_CALL_H_
