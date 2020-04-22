#ifndef _INCRIP_CACHE_H_
#define _INCRIP_CACHE_H_



extern OPCODE pContext;
//extern OPCODE pContext2;
extern OPCODE pStack2;
extern OPCODE pDocker;

extern QWORD addressIncRip;
extern OPCODE traceLoopData;
extern OPCODE traceLoopPreStart;
extern OPCODE traceLoopStart;
extern OPCODE traceLoopTraceINSTRUCTION;
extern OPCODE traceLoopEnd;


/*extern OPCODE saveRsp;
extern OPCODE loadR10;
extern OPCODE saveR15;
extern OPCODE loadR15;*/

/*inline void setCacheEntry(DOCKER * docker) {
	DWORD memoryCheckValue;
	for (DWORD i = 0; i < ACCESS_ADDRESS_SIZE; i += 2) {
		if (docker->accessAddress[i + 1] == docker->Rip && docker->accessAddress[docker->lastAccessAddressIndex] != docker->Rip) {
			memoryCheckValue = memoryCheck(&docker->memBasicInfo, docker->Rip, BUFFER_Opcode_SIZE);
			if (memoryCheckValue == PAGE_EXECUTE_READWRITE) {
				break;
			}
			docker->entryCacheIndex = i;
			break;
		}
	}
}*/


//size 35
/*inline void placecallback(void * callback, BYTE * buffer, void *callbackTransfer) {
	ULONG_PTR jmpRet;
	ULONG_PTR jmpPtr;

	if (callback == NULL) {
		return;
	}
	jmpRet = ((ULONG_PTR)callbackTransfer + (ULONG_PTR)&traceLoopTraceINSTRUCTION - (ULONG_PTR)&traceLoopPreStart);
	jmpPtr = ((ULONG_PTR)callbackTransfer + (ULONG_PTR)&addressIncRip - (ULONG_PTR)&traceLoopPreStart);


	//set the ret jump
	movDword(buffer, jmpRet + 1, (DWORD)(((ULONG_PTR)(buffer + callback_SIZE) - (ULONG_PTR)(jmpRet + 5) & MAXDWORD)));

	//set the x64 jump
	movQword(buffer + 10, jmpPtr, callback);

	//jump to the callback
	jmpRel32(buffer + 30, callbackTransfer);

}*/

void addFunctionToCache(OPCODE * entryPoint, DOCKER * docker);
BOOL MovEntireFunction(_Inout_ ANALYSED_FUNCTION * analyzedFunction, _Out_opt_ OPCODE ** accessAddress, _Inout_opt_ DWORD * accessAddressIndex_, _In_opt_ DWORD accessAddressSize, _In_ void(*INSTRUCTIONcallback)(), _In_ void(*exitFunctioncallback)(), _In_ QWORD * newRip, _Inout_ QWORD * saveZone);


#endif // !_INCRIP_CACHE_H_
