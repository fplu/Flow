#ifndef _MODULE_MAPPING_H_
#define _MODULE_MAPPING_H_

typedef struct PAGE_CALL_INFO_s {//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS
	QWORD OldRetAddress;
	QWORD NewRetAddress;
	void(*RetCallback)(void*);
}PAGE_CALL_INFO, *PPAGE_CALL_INFO;

typedef struct MAPPED_INSTRUCTION_MINIMAL_INFO_s {
	QWORD OldAddress;
	QWORD NewAddress;
	void(*Callback)(void*);
}MAPPED_INSTRUCTION_MINIMAL_INFO;

typedef struct MAPPED_INSTRUCTION_s{
	union {
		ANALYSED_INSTRUCTION;
		ANALYSED_INSTRUCTION AInstruction;
	};
	BOOLEAN  IsUnMapped;
	void(*Callback)(void*);
	QWORD Dest;
}MAPPED_INSTRUCTION;

typedef struct PAGE_RANGE_s {//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS
	QWORD BaseAddress;
	QWORD RegionSize;
}PAGE_RANGE, *PPAGE_RANGE;

#define PAGE_RANGE_BUFFER_DEFAULT_LENGTH 10//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS

typedef struct PAGE_RANGE_BUFFER_s {//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS
	PAGE_RANGE Pages[PAGE_RANGE_BUFFER_DEFAULT_LENGTH];
	QWORD MaxLength;
	QWORD Length;
}PAGE_RANGE_BUFFER, * PPAGE_RANGE_BUFFER;

typedef struct MAPPED_PAGE_s {//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS
	OPCODE * NewCode;
	OPCODE * OldCode;
	DWORD * OldCodeMask;
//	DWORD * NewToId;
//	DWORD * OldToId;
	DWORD NewCodeLength;
	DWORD OldCodeLength;
	DWORD OldCodeMaskLength;

//	PAGE_RANGE_BUFFER OldDatas;

	MAPPED_INSTRUCTION_MINIMAL_INFO * InstructionsMinimal;
	MAPPED_INSTRUCTION_MINIMAL_INFO * Instructions;
	DWORD InstructionsLength;
	DWORD InstructionsMinimalLength;

	PPAGE_CALL_INFO CallInfo;
	DWORD CallInfoLength;

	QWORD* NewData;
	DWORD NewDataLength;

	DWORD OriginalProtect;

	BOOL IsModuleAnalysed;
	BOOL IsModuleModifiable;
}MAPPED_PAGE, *PMAPPED_PAGE;

typedef struct MAPPED_MODULE_MANAGEMENT_s {
	MAPPED_PAGE * Element;//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS
	DWORD ElementLength;//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS
	DWORD ElementMaxLength;
	LONG SynchronizationByte; //WARNING SOME ASSEMBLY STUFF DEPEND ON THIS//Check the synchronization byte before reading or writing, 0 reading authorized, set 1 to write, 1 wait
	DWORD WastedElementLength;
	MAPPED_PAGE* WastedElement;
	BOOLEAN AreAllModuleInit;
	PVOID Callback;
	PVOID Cookie;			//Cookie from LdrRegisterDllNotification
}MAPPED_MODULE_MANAGEMENT;

typedef struct CALLBACK_LIST_s {
	QWORD** ppExit, ** ppCallback, ** ppRet, **ppRet2, ** ppJmp, ** ppCall, ** ppCall2, ** ppSyscall;
}CALLBACK_LIST, * PCALLBACK_LIST;

typedef struct MAPPED_MODULE_PROCESSING_s {
	OPCODE* OldAddress;
	DWORD OldSize;
	BOOLEAN BeingProcessed;
}MAPPED_MODULE_PROCESSING, *PMAPPED_MODULE_PROCESSING;


extern MAPPED_MODULE_MANAGEMENT g_mmm;
extern PMAPPED_PAGE* g_ppmp;
extern __declspec(thread) MAPPED_MODULE_PROCESSING g_mmp;

#define g_ppmp(address) g_ppmp[((((QWORD)(address)) & 0xFFFFFF000) >> 12)]

inline BOOL IsOldModuleAddressInModule(_In_ MAPPED_PAGE * mappedModule, _In_ QWORD oldModuleAddress) {
	if ((QWORD)mappedModule->OldCode >= oldModuleAddress && (QWORD)mappedModule->OldCode + (QWORD)mappedModule->OldCodeLength < oldModuleAddress) {
		return TRUE;
	}
	return FALSE;
}

//WARNING DEPEND ON SOME ASSEMBLY
inline QWORD GetNewModuleAddressFromOldModuleAddress(_In_ MAPPED_PAGE * mappedModule, _In_ QWORD oldModuleAddress) {
	DWORD tmp = mappedModule->OldCodeMask[oldModuleAddress - (QWORD)mappedModule->OldCode];
	if (tmp == NO_LINKED_INSTRUCTION) {
		return (QWORD)NULL;
	}
	return (QWORD)tmp + (QWORD)mappedModule->NewCode;
}



QWORD ModuleMappingMap(_In_ OPCODE * baseAddress, _In_ DWORD baseSize, _Inout_ MAPPED_PAGE * mappedModule, _In_ BOOL analyzeModule);
void ModuleMappingOnNewModuleLoad(_In_ ULONG NotificationReason, _In_ PLDR_DLL_NOTIFICATION_DATA NotificationData, _In_opt_ PVOID Context);
BOOL ModuleMappingInit(_In_ PVOID callback);
void ModuleMappingMapPage(OPCODE* baseAddress, DWORD baseSize);
BOOL ModuleMappingMapAllUnmappedModule(void);
//QWORD GetNewAddressFromOldAddress(_In_ QWORD oldAddress, _In_ BOOL sync);
BOOL IsAddressInDataPage(PMAPPED_PAGE pPage, QWORD address);

QWORD ModuleMappingGetNewAddressFromOldAddress(_Inout_ volatile DWORD*  pFlag, _In_ QWORD oldAddress, _In_ BOOL sync);

BOOL FlowSuspendThread(DWORD threadId);
BOOL FlowResumeThread(DWORD threadId);
BOOLEAN ModuleMappingClearModule(DWORD moduleIndex);


#endif // !_MODULE_MAPPING_H_
