#ifndef _INC_RIP_EXPORT_H_
#define _INC_RIP_EXPORT_H_


#define CALL_BACK_STEP 0x1
#define CALL_BACK_EXECUTION_ACCESS_VIOLATION 0x2
#define CALL_BACK_MISCALLENOUS_EXCEPTION 0x3
#define CALL_BACK_ACCESS_RIGHT_MODIFICATION 0x4

#ifndef EMULATOR_HANDLER
typedef void* EMULATOR_HANDLER;//user name for docker, do not use docker directly
#endif // !EMULATOR_HANDLER


/*
* The field returned located MAPPED_MODULE_PROCESSING.BeingProcessed allow to know if we are currently disassembling a memory region. It is set to TRUE if we are disassembling a memory region, FALSE otherwise.
*/
_LIB_INC_RIP_API_ MAPPED_MODULE_PROCESSING getMappingState(void);

/*
* Allow to start the analyze of the thread on which this function is running. The main thread is analyzed by default. You could put it in DLL_THREAD_ATTACH to analyze newly created thread.
*/
_LIB_INC_RIP_API_ void debugCurrentThread(void);

/**
 * Init flow in the current process, but do not start it. If you are calling this function manually you should consider calling debugCurrentThread on the main thread of the process.
 *
 *
 * @param[in] modulesNames table of all module path which make 500 byte length. It length should be equal to numberOfModule * 500 at least.
 * @param[in] numberOfModule number of module. Between 0 and 10 or it fails.
 */
_LIB_INC_RIP_API_ void globalFlowInit(_In_ WCHAR * modulesNames, _In_ DWORD numberOfModule);

/*
* Add a syscall callback. This function do not contains sync between thread, it is unsafe with removeSyscallCallback.
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] _syscallCallback callback function to called before each syscall if eax == syscallId.
* @param[in] syscallId value of eax before the syscall instruction is executed.
*/
_LIB_INC_RIP_API_ void addSyscallCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ syscallCallback _syscallCallback, _In_ DWORD syscallId);

/*
* Remove a syscall callback. This function do not contains sync between thread, it is unsafe with removeSyscallCallback. The condition between the parameter is a AND.
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] _syscallCallback remove all syscall callback if the callback address is equal to _syscallCallback. This field is ignored if NULL.
* @param[in] syscallId remove all syscall callback if their id is equal to syscallId. This field is ignored if -1.
*/
_LIB_INC_RIP_API_ void removeSyscallCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_opt_ DWORD syscallId, _In_opt_ syscallCallback _syscallCallback);

/*
* Get the context of the instrumented thread.
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[out] threadContext pointer to the context that receive the context.
*/
_LIB_INC_RIP_API_ void getThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _Out_ THREAD_CONTEXT * threadContext);

/*
* Set the context of the instrumented thread. WARNING DO NOT SET ALL REGISTERS. 
* Does not set R14-R12, RSI, RDI, RBX, RBP. 
* Sets R15, R11-R8, RAX, RCX, RDX, RSP, XMM0-5
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] threadContext pointer to the context to be set in the current thread.
*/
_LIB_INC_RIP_API_ void setThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ THREAD_CONTEXT * threadContext);

/*
* Set the context of the instrumented thread
* when use MUST :
*		- be called first by the Callback function
*		- the Callback function must not modify segment register
*		- the Callback function must not use floating operation or other xmm, ymm and zmm operation
*		- the Callback function could call function that use floating operation or other xmm, ymm and zmm operation
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] threadContext pointer to the context that receive the context.
*/
_LIB_INC_RIP_API_ void getExtendedThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _Out_ CONTEXT * threadContext);

/*
* Set the context of the instrumented thread..
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] threadContext pointer to the context to be set in the current thread.
*/
_LIB_INC_RIP_API_ void setExtendedThreadContext(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ CONTEXT * threadContext);

//_LIB_INC_RIP_API_ void setThreadDefaultCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ void(*threadDefaultCallback)(EMULATOR_HANDLER *));

/*TODO*/
_LIB_INC_RIP_API_ BOOL addCallCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ void * function, _In_ void(callback_)(EMULATOR_HANDLER* docker, QWORD lastRip));
/*TODO*/
_LIB_INC_RIP_API_ BOOL addRetCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ void * function, _In_ void(callback_)(EMULATOR_HANDLER* docker, QWORD lastRip));

/**
* You can use the TLS provided by FLOW instead of the one of Windows. You have up to 256 slot.
* Allocate one slot inside the TLS.
* 
* @param[out] pTlsId id of the allocated slot, -1 mean the allocation fail.
*/
_LIB_INC_RIP_API_ void tlsAlloc(_Out_ DWORD * pTlsId);

/**
* You can use the TLS provided by FLOW instead of the one of Windows. You have up to 256 slot.
* Free one slot inside the TLS.
*
* @param[out] tlsId id of the slot to free.
*/
_LIB_INC_RIP_API_ void tlsFree(_In_ DWORD tlsId);

/**
* You can use the TLS provided by FLOW instead of the one of Windows. You have up to 256 slot.
* Set the value of a slot inside the TLS for the current thread (given by pEmulatorHandler).
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] tlsId id of the slot to set.
* @param[in] value value to set.
*/
_LIB_INC_RIP_API_ void tlsSetValue(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ DWORD tlsId, _In_ QWORD value);


/**
* You can use the TLS provided by FLOW instead of the one of Windows. You have up to 256 slot.
* Set the value of a slot inside the TLS for the current thread (given by pEmulatorHandler).
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] tlsId id of the slot to get.
* @param[out] pValue the value of the slot.
*/
_LIB_INC_RIP_API_ void tlsGetValue(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_ DWORD tlsId, _Out_ QWORD * pValue);

/*
* you MUST call this function only inside a syscall handler. A post call back for the syscall will happen ONCE with the desired function
* it fail if a callback was already set
*
* @param[in] pEmulatorHandler handle to FLOW.
* @param[in] postCallback function to call after the syscall, only once.
*/
_LIB_INC_RIP_API_ BOOL setSyscallPostCallback(_Inout_ EMULATOR_HANDLER * pEmulatorHandler, _In_opt_ void(*postCallback)(EMULATOR_HANDLER*));


/*
* Used internally
*/
void _getAllRegister(CONTEXT * context);

#endif // !_INC_RIP_EXPORT_H_
