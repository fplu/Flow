#ifndef _INJECTEUR_INSTRUCTION_POINTER_MANAGEMENT_H_
#define _INJECTEUR_INSTRUCTION_POINTER_MANAGEMENT_H_

_LIB_INC_RIP_API_ BOOL injectEmulatorAndUserDll(_In_ const TCHAR * const * const dllNames, _In_ const DWORD dllNamesLength, _In_ const TCHAR * const exePath, _In_opt_ TCHAR * exeCommandLine);
_LIB_INC_RIP_API_ BOOL prepareInjectorFunction(const CONTEXT * context, _In_ const TCHAR * const * const dllPaths, DWORD dllPathsLenght);
_LIB_INC_RIP_API_ BOOL executeInjectorFunction1(HANDLE hProcess, HANDLE hThread, _In_ const TCHAR * const * const dllPaths, _In_ DWORD dllPathsLength);
_LIB_INC_RIP_API_ BOOL executeInjectorFunction2(HANDLE hProcess, HANDLE hThread, _In_ const TCHAR * const * const dllPaths, _In_ DWORD dllPathsLength);



#endif // !_INJECTEUR_INSTRUCTION_POINTER_MANAGEMENT_H_
