#ifndef _EXIT_MAPPING_H_
#define _EXIT_MAPPING_H_

/*
*	Used internally
*/
inline DWORD GetMappedExitMappingSize(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, DWORD* pIndex) {
	(*pIndex)++;
	return 12 /*Arg*/ + 6 /*callback*/;
}


/*
*	Used internally
*/
inline DWORD WriteMappedExitMapping(PMAPPED_PAGE pPage, MAPPED_INSTRUCTION* pInstruction, OPCODE* pBuffer, void(*exit)(void), DWORD tlsId, DWORD* pIndex) {
	movDwordToTls(pBuffer, tlsId, *pIndex);
	jmpAtAddress(pBuffer + 12, exit);
	pInstruction->IsUnMapped = TRUE;

	return GetMappedExitMappingSize(pPage, pInstruction, pIndex);
}

#endif // !_EXIT_MAPPING_H_
