#include "FlowMain.h"

extern void TAKE_THIS_(void);

_LIB_INC_RIP_API_ void * TAKE_THIS(void);

void * TAKE_THIS(void) {
	return TAKE_THIS_;
}

BOOL injectNewThread = TRUE;
extern QWORD TmpTlsId;
//extern DWORD g_DockerTlsId;
extern OPCODE StartGoCallback;
extern OPCODE StartCodeGoCallback;
extern OPCODE EndGoCallback;
extern OPCODE setRcx;


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH) {
		DWORD oldProtect;
		VirtualProtect(
			&traceLoopPreStart,
			&traceLoopEnd - &traceLoopPreStart,
			PAGE_EXECUTE_READWRITE,
			&oldProtect
		);
		*(QWORD*)(&addressIncRip) = (QWORD)SingleStepHandlerRun;
		VirtualProtect(
			&traceLoopPreStart,
			&traceLoopEnd - &traceLoopPreStart,
			oldProtect,
			&oldProtect
		);

		g_TlsIdDocker = TlsAlloc();
		g_TlsIdArg = TlsAlloc();
		g_TlsIdStoreR1 = TlsAlloc();
		g_TlsIdStoreR2 = TlsAlloc();
		g_TlsIdStoreR3 = TlsAlloc();
		g_TlsIdStorePage = TlsAlloc();

		//g_TlsIdStoreCallStackLength = TlsAlloc();
		//g_TlsIdStoreCallStack = TlsAlloc();


		/*VirtualProtect(
			&TmpTlsId,
			&EndGoCallback - &StartGoCallback,
			PAGE_EXECUTE_READWRITE,
			&oldProtect
		);
		TmpTlsId = g_TlsIdDocker;*/

		if (!ModuleMappingInit(&StartCodeGoCallback)) {
			return FALSE;
		}
	}

	if (fdwReason == DLL_THREAD_ATTACH) {
		//choice of the user
		//TAKE_THIS_();
	}
	
	
	//hookKiUserExceptionDispatchSignature
	return TRUE;
}

