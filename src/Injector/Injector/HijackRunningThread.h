#ifndef _HIJACK_RUNNING_THREAD_
#define _HIJACK_RUNNING_THREAD_

BOOL initRemoteRip();
BOOL synchronizeThread(_In_ HANDLE hRemoteProcess, _In_ HANDLE hRemoteThread);
BOOL takeThreadControl(_In_ HANDLE hRemoteProcess, _In_ HANDLE hRemoteThread, _Outptr_ CONTEXT * pInitialContext);
BOOL releaseThreadControl(_In_ HANDLE hRemoteThread, _In_  CONTEXT * pInitialContext);
BOOL CLR_Wait(_In_ HANDLE hRemoteThread, _In_  CONTEXT * pInitialContext);

BOOL remoteCall(_In_ HANDLE hRemoteThread, _In_ const CONTEXT * pInitialContext, _In_ void * function, _In_opt_ void * arg1, _In_opt_ void * arg2, _In_opt_ void * arg3, _In_opt_ void * arg4, _Outptr_ void ** retValue);

BOOL myVirtualAllocEx(_In_ HANDLE hRemoteThread, _In_ const CONTEXT * pInitialContext, _In_ DWORD bufferSize, _Out_ BYTE ** pRemoteBuffer);
BOOL myWriteProcessMemory(_In_ HANDLE hRemoteThread, _In_ const CONTEXT * pInitialContext, _In_ BYTE * buffer, _In_ DWORD bufferSize, _In_ BYTE * remoteBuffer);
BOOL writeAndAllocProcessMemory(_In_ HANDLE hRemoteProcess, _In_ HANDLE hRemoteThread, _In_ BYTE * buffer, _In_ DWORD bufferSize, _Out_ BYTE ** pRemoteBuffer);

BOOL remoteGetProcAddress(_In_ HANDLE hRemoteThread, _In_ const CONTEXT * pInitialContext, _In_ char * arg1, _In_ char * arg2, _Outptr_ void ** pFunction);
BOOL remoteLoadLibrary(_In_ HANDLE hRemoteThread, _In_  const CONTEXT * pInitialContext, _In_ const TCHAR * dllPath);

BOOL CreateCallOnTheFly(_In_ HANDLE hRemoteThread, _In_ const CONTEXT * pInitialContext, _In_ void * function, _In_opt_ void * arg1, _In_opt_ void * arg2, _Outptr_ void ** page);
BOOL HijackRunningThread(_In_ HANDLE hRemoteThread, _In_ CONTEXT * pInitialContext, _In_ void * function, _In_opt_ void * arg1, _In_opt_ void * arg2, _Outptr_ void ** page);


#endif // !_HIJACK_RUNNING_THREAD_
