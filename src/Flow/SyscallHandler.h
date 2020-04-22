#ifndef _SYSCALL_HANDLER_H_
#define _SYSCALL_HANDLER_H_

/**
 * Called this function if you want to enable the default syscall handling
 */
_LIB_INC_RIP_API_ void SyscallHandlerInit(void);

/*
*	Default syscall callback for NtContinue. It allows to keep the execution flow even when a NtContinue syscall is met.
*/
_LIB_INC_RIP_API_ BOOL callbackNtContinue(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	Default syscall callback for NtProtectVirtualMemory. It allows to ensure the cache consistency. In other word, it allow to detect a changement of right on a module.
*/
_LIB_INC_RIP_API_ BOOL callbackNtProtectVirtualMemory(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

//_LIB_INC_RIP_API_ BOOL callbackNtAllocateVirtualMemory(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	You should enable this syscall callback on NtCreateUserProcess with callbackNtResumeThreadForCreateProcess to follow child process.
*/
_LIB_INC_RIP_API_ BOOL callbackNtCreateUserProcess(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	You should enable this syscall callback on NtResumeThread with callbackNtCreateUserProcess to follow child process.
*/
_LIB_INC_RIP_API_ BOOL callbackNtResumeThreadForCreateProcess(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	Allow to set the context to the real one instead of suspending during the instrumentation. It should be used with callbackNtResumeThread
*/
_LIB_INC_RIP_API_ BOOL callbackNtSuspendThread(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	Allow to set the context to the real one instead of suspending during the instrumentation. It should be used with callbackNtSuspendThread
*/
_LIB_INC_RIP_API_ BOOL callbackNtResumeThread(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	Used internally by callbackNtCreateUserProcess
*/
_LIB_INC_RIP_API_ BOOL postCallbackNtCreateUserProcess(_Inout_ EMULATOR_HANDLER * pEmulatorHandler);


#define g_SyscallCallbackMaxLength 500

typedef BOOL(*syscallCallback)(_In_ THREAD_CONTEXT* context, _Inout_ QWORD* rip, _Inout_ EMULATOR_HANDLER* pEmulatorHandler, _Inout_ OPCODE* buffer, _Inout_ DWORD* bufferIndex);

/*
*	Used internally, WARNING ASSEMBLY STUFF DEPEND ON THIS STRUCT
*/
typedef struct SYSCALL_CALLBACK_ {
	syscallCallback address;
	DWORD id;
}SYSCALL_CALLBACK;

/*
*	Used internally, WARNING ASSEMBLY STUFF DEPEND ON THIS STRUCT
*/
extern SYSCALL_CALLBACK g_SyscallCallback[g_SyscallCallbackMaxLength];

/*
*	Used internally, WARNING ASSEMBLY STUFF DEPEND ON THIS STRUCT
*/
extern DWORD g_SyscallCallbackLength;

/*
*	Used internally, WARNING ASSEMBLY STUFF DEPEND ON THIS STRUCT
*/
extern syscallCallback g_SyscallDefaultCallback;


#endif // !_SYSCALL_HANDLER_H_
