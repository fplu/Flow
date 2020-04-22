#ifndef _RIP_JMP_H_
#define _RIP_JMP_H_

/*inline QWORD MappedJmpModRmRipRel(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction) {
	if (IS_MODRM_RIP_RELATIVE(pInstruction->Opcode[pInstruction->ModRMPos])) {
		QWORD * pDest;

		if (pInstruction->AddressSize != 8) {
			return NULL;
		}
		
		pDest = (QWORD*)((BYTE*)pInstruction->Opcode + *(int*)(pInstruction->Opcode + pInstruction->ModRMPos + 1) + pInstruction->Length);

		if (IsAddressInDataPage(pPage, pDest)) {
			return GetNewAddressFromOldAddress(NULL, *pDest, FALSE);
		}
	}
	return NULL;
}*/

inline DWORD GetMappedJmpSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		/*if (MappedJmpModRmRipRel(pPage, pInstruction) != NULL) {
			pPage->NewDataLength++;
			return 6;
		}*/

		if (pInstruction->ModRMPos == 255) {
			return GetMappedExitMappingSize(pPage, pInstruction, pIndex);
		}

		BYTE modrmSize = GetModRmSize(pInstruction->Opcode + pInstruction->ModRMPos);
		DWORD i = 0;
		
		i += 9;
		if (IS_MODRM_RIP_RELATIVE(pInstruction->Opcode[pInstruction->ModRMPos])) {
			i += 13;
		}
		else {
			i += 3 + modrmSize;
		}
		i += 18;
		return GetMappedExitMappingSize(pPage, pInstruction, pIndex) + i;

	}
	return 5;
}


inline DWORD WriteMappedJmp(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* buffer, PCALLBACK_LIST pCallbackList, DWORD tlsId, DWORD* pIndex) {
	if (pInstruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
		QWORD dest;
		/*if ((dest = MappedJmpModRmRipRel(pPage, pInstruction)) != NULL) {
			pPage->NewData[pPage->NewDataLength] = dest;
			jmpAtAddress(buffer, pPage->NewData + pPage->NewDataLength);
			return GetMappedJmpSize(pPage, pInstruction, pIndex);;
		}*/
		
		if (pInstruction->ModRMPos == 255) {
			return  WriteMappedExitMapping(pPage, pInstruction, buffer, (void(*)(void))(void*)pCallbackList->ppExit, tlsId, pIndex);
		}

		MODRM modrm;
		BYTE modrmSize = GetModRmSize(pInstruction->Opcode + pInstruction->ModRMPos);
		DWORD i = 0;
		REX rex;
		
		rex.EntireByte = 0x48;
		rex.B = pInstruction->Rex.B;
		rex.X = pInstruction->Rex.X;
		modrm = *(MODRM*)(pInstruction->Opcode + pInstruction->ModRMPos);
		modrm.Reg = RDI_ID;


		movRdiToTls(buffer + i, g_TlsIdStoreR1); i += 9;
		if (IS_MODRM_RIP_RELATIVE(pInstruction->Opcode[pInstruction->ModRMPos])) {
			dest = (QWORD)(QWORD*)((BYTE*)pInstruction->Opcode + *(int*)(pInstruction->Opcode + pInstruction->ModRMPos + 1) + pInstruction->Length);
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
		movRdiToTls(buffer + i, g_TlsIdStoreR2); i += 9;
		movTlsToRdi(buffer + i, g_TlsIdStoreR1); i += 9;
//		buffer[30 + modrmSize] = 0xCC;
//		__debugbreak();

		return WriteMappedExitMapping(pPage, pInstruction, buffer + i, (void(*)(void))(void*)pCallbackList->ppJmp, tlsId, pIndex) + i;
	}

	jmpRel32(buffer, pInstruction->Dest);
	*(PBYTE)(buffer) = (UCHAR)0xE9;
	return GetMappedJmpSize(pPage, pInstruction, pIndex);
}


BOOL ManageJmp(
	_In_ DOCKER* docker,
	_In_opt_ THREAD_CONTEXT* context,
	_In_ INSTRUCTION_READ* instructionRead,
	_Inout_ OPCODE* buffer,
	_Inout_ DWORD* bufferIndex,
	_In_ DWORD bufferLength
);

#endif // !_RIP_JMP_H_
