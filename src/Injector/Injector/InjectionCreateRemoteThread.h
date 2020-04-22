#ifndef _INJECTION_CREATE_REMOTE_THREAD_H_
#define _INJECTION_CREATE_REMOTE_THREAD_H_



BOOL injectionCreateRemoteThread(_In_ PTCHAR processName, _In_ PTCHAR dll32, _In_ PTCHAR dll64, _In_ BOOL isPath, _Out_ PHANDLE remoteThread, _Out_ PPROCESS_INFORMATION processInformation);
BOOL OpenProcessForHook(_In_ LPCTSTR path, _In_opt_ LPTSTR commandLine, _Out_ PBOOL is32BitProcess, _Out_ PPROCESS_INFORMATION processInformation);
BOOL GetProcessByName(_In_ LPTSTR processName, _Out_ PBOOL is32BitProcess, _Out_ PHANDLE hprocess);
BOOL DLLInject(_In_ HANDLE hProcess, _In_ TCHAR * dllPath, _In_ BOOL is32BitProcess, _Out_ HANDLE *remoteThread);
BOOL get32BitAdress(_In_ WCHAR * dllName, _In_ WCHAR * functionName, _Out_ PDWORD adresse32Bit);

#endif // !_INJECTION_CREATE_REMOTE_THREAD_H_
