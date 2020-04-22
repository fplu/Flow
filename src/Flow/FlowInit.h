#ifndef _FLOW_INIT_H_
#define _FLOW_INIT_H_

#define DOCKER_TLS_SIZE 256
#define CACHE_SIZE 16
#define ACCESS_ADDRESS_SIZE (CACHE_SIZE*8)
#define MAX_BREAK_POINT 256
#define MAX_THREAD 256
#define DOCKER_ASK_SUSPENDED (0x0001)
#define PAGE_CALL_INFO_SIZE (0x100)//WARNING SOME ASSEMBLY STUFF DEPEND ON THIS (moreover we need to get the size of a (MAX_UNSIGNED_CHAR + 1))

typedef void* EMULATOR_HANDLER;//user name for docker, do not use docker directly. This structure change for each thread


typedef struct DOCKER_ {
	void* Rip;
	void* rien;
	THREAD_CONTEXT context;
	CONTEXT fullContext;
	OPCODE* codeBaseAddress;
	OPCODE* traceLoopStart;
	OPCODE* traceLoopTraceInstruction;
	OPCODE* traceLoopEnd;
	QWORD instructionId;
	union {//could be both but never at the same time
		MAPPED_INSTRUCTION* mappedInstruction;
		MAPPED_INSTRUCTION_MINIMAL_INFO* mappedInstructionMinimal;
	};
	OPCODE* callbackBaseAddress;

	QWORD myGs;//not entire QWORD is used but who cares ?

	void* lastRip;//for exception handler
	void* oldRip;
	void* memRip;//use for debugging only

	DWORD cpt;

	QWORD saveZone;//help to store register without the use of the stack
	void(*syscallPostCallback)(struct DOCKER_*); //allow to execute syscall post call back, when use you must set isStepEnable to TRUE


	MAPPED_MODULE_MANAGEMENT mmm;

	volatile DWORD Flag;
	DWORD SuspendCount;
	DWORD OFFSET_ADDRESS_PTR;
	DWORD OFFSET_NEW_CONTEXT_PTR;
	DWORD OFFSET_START_LOAD_COMPLETE_CONTEXT;

	PPAGE_CALL_INFO pageCallInfo;
	MEMORY_BASIC_INFORMATION memBasicInfo;
	QWORD tls[DOCKER_TLS_SIZE];


	int ptitCountPtetTemp;
	int ptitCountPtetPasTemp;
}DOCKER;

_LIB_INC_RIP_API_ QWORD initIncRip(_In_ void* _firstRip, _In_opt_ void* _firstStack);

extern DWORD g_TlsIdDocker;
extern DWORD g_TlsIdArg;
extern DWORD g_TlsIdStoreR1;
extern DWORD g_TlsIdStoreR2;
extern DWORD g_TlsIdStoreR3;
extern DWORD g_TlsIdStorePage;

extern BYTE  g_TlsAllocationMask[DOCKER_TLS_SIZE];

typedef struct DOCKER_THRAD_ID_s{
	DOCKER* Docker;
	DWORD ThreadId;
	LONG Sync;
}DOCKER_THRAD_ID, * PDOCKER_THRAD_ID;
extern DOCKER_THRAD_ID g_dockerThreadId[100];
extern DWORD g_dockerThreadIdLength;
extern DWORD g_dockerThreadIdSync;


/*
*	If a function is named "processInitializationCallback" in the user dll it will ba called at process initialization outside of a dllMain
*/
extern void(*g_ProcessInitializationCallback)(void);
extern BOOLEAN isProcessInitializationDone;

/*
*	If a function is named "threadInitializationCallback" in the user dll it will ba called at each time a new thread start to be instrumented
*/
extern void(*g_ThreadInitializationCallback)(EMULATOR_HANDLER*);

/*
*	If a function is named "threadInitializationCallback" in the user dll it will ba called at each time a new thread start to be instrumented
*
*	The first param is a handle to FLOW.
*/
extern void(*g_InstructionDefaultCallback)(EMULATOR_HANDLER*);



/*
*	If a function is named "getInstructionCallback" in the user dll it will ba called at each time an instruction is instrumented. 
*	It take the current instruction as parameter and return a pre callback for this instruction.
*	The pre callback will receive a handle to Flow as first parameter each time it is called
*/
extern void(*(*g_GetInstructionCallback)(INSTRUCTION_READ*))(EMULATOR_HANDLER*);

/*
*	If a function is named "exceptionCallback" it will be called each time an unexpected behaviour is detected passing OPTIONNALY a handle to FLOW or optionnaly a context to the code which cause the exception and the reason of exception
*/
extern void(*g_ExceptionCallback)(EMULATOR_HANDLER*, CONTEXT*, DWORD);

/*
*	If a function is named "enableModuleInstructionAnalyse" in the user dll, it should return wether or not we enable all callbacks in a region of memory passed as argument (starting to baseAddress to baseAddress + baseSize).
*	TRUE the callback are enable. FALSE the callback are disable
*	When this function is not set we consider all callback are enable.
*/
extern BOOL(*g_EnableRangeInstructionAnalyseFunction)(OPCODE* baseAddress, DWORD baseSize);

#endif // !_FLOW_INIT_H_
