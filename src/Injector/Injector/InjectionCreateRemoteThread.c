#include "InjectorMain.h"

#define _32BITGETADRESS L"..\\Debug\\get32BitAdress.exe"

BOOL injectionCreateRemoteThread(_In_ PTCHAR processName, _In_ PTCHAR dll32, _In_ PTCHAR dll64, _In_ BOOL isPath, _Out_ PHANDLE remoteThread, _Out_ PPROCESS_INFORMATION processInformation) {
	BOOL sucess = TRUE;

	HANDLE hProcess = NULL;
	BOOL is32BitProcess = 0;

	__try {
		if (!processName) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}


		//Etape 1 : On cherche le process à hook ou on l'ouvre en pause.
		if (isPath) {
			if (!OpenProcessForHook(processName, NULL, &is32BitProcess, processInformation)) {
				sucess = FALSE;
				MSDN_FUNC_ERROR();
			}
			hProcess = processInformation->hProcess;
		}
		else {
			if (!GetProcessByName(processName, &is32BitProcess, &hProcess)) {
				sucess = FALSE;
				MSDN_FUNC_ERROR();
			}
		}

		//Etape 2 : On met en place le hook.
		if (is32BitProcess && !DLLInject(hProcess, dll32, is32BitProcess, remoteThread) || !is32BitProcess && !DLLInject(hProcess, dll64, is32BitProcess, remoteThread)) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}

	}
	__finally {
		if (hProcess) { // == processInformation->hProcess
			CloseHandle(hProcess);
		}
	}
	return sucess;
}


BOOL OpenProcessForHook(_In_ LPCTSTR path, _In_opt_ LPTSTR commandLine, _Out_ PBOOL is32BitProcess, _Out_ PPROCESS_INFORMATION processInformation) {
	BOOL success = TRUE;
	STARTUPINFO systemInformation;
	BOOL is64Bit = 0;	
	DWORD i;
	LPTSTR dirPath = NULL;

	__try {
		ZeroMemory(&systemInformation, sizeof(systemInformation));
		systemInformation.cb = sizeof(systemInformation);

		if (!path || !is32BitProcess || !processInformation) {
			SetLastError(ERROR_INVALID_PARAMETER);
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		for (i = 0; path[i] != _T('\0'); i++);
		if ((dirPath = malloc(((size_t)i+1)*sizeof(TCHAR))) == NULL) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		for (i = 0; path[i] != _T('\0'); i++) {
			dirPath[i] = path[i];
		}
		for (; dirPath[i] != _T('\\') && dirPath[i] != _T('/') && i >= 0; i--);
		if (dirPath[i] == _T('\0')) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}

		dirPath[i + 1] = _T('\0');


		int a;
		if (!(a = CreateProcess(path, commandLine, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, dirPath, &systemInformation, processInformation))) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}


		if (!IsWow64Process(processInformation->hProcess, &is64Bit)) {
			success = FALSE;
			MSDN_FUNC_ERROR();
		}
		if (is64Bit == TRUE) {
			_tprintf(_T("32 bits process\n"));
			(*is32BitProcess) = 1;
		}
		else {
			_tprintf(_T("64 bits process\n"));
			(*is32BitProcess) = 0;
		}

	}
	__finally {
		if (dirPath) {
			free(dirPath);
		}
	}

	return success;
}

//Renvoie l'handle du processus demandé.
BOOL GetProcessByName(_In_ LPTSTR processName, _Out_ PBOOL is32BitProcess, _Out_ PHANDLE hprocess) {
	BOOL sucess = TRUE;
	HANDLE processList = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 currentProcess = { 0 };
	BOOL is64Bit = 0;

	__try {
		if (!processName || !is32BitProcess || !hprocess) {
			SetLastError(ERROR_INVALID_PARAMETER);
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}

		//Initialise et charge une liste contenant l'ensemble des processus en cours d'execution.
		if ((processList = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) == INVALID_HANDLE_VALUE) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}

		//Indique la taille de la structure pour savoir quelle version utilisé.
		currentProcess.dwSize = sizeof(currentProcess);

		//Charge le premiere element de la liste sur currentProcess.
		if (!Process32First(processList, &currentProcess)) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}

		do {
			//Compare l'element courant et l'element recherché.
			if (!_tcscmp(processName, currentProcess.szExeFile)) {
				//Si ok on renvoie l'handle du processus recherché, ou NULL si cela échoue.
				if (!(*hprocess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, currentProcess.th32ProcessID))) {
					sucess = FALSE;
					MSDN_FUNC_ERROR();
				}
				if (!IsWow64Process(*hprocess, &is64Bit)) {
					sucess = FALSE;
					MSDN_FUNC_ERROR();
				}
				if (is64Bit == TRUE) {
					_tprintf(_T("32 bits process\n"));
					(*is32BitProcess) = 1;
				}
				else {
					_tprintf(_T("64 bits process\n"));
					(*is32BitProcess) = 0;
				}
				__leave;
			}
			//tant que pas pareil on vas au processus suivant.
		} while (Process32Next(processList, &currentProcess));
		sucess = FALSE;
		MSDN_FUNC_ERROR();
	}
	__finally {
		if (processList) {
			CloseHandle(processList);
		}
		if (!sucess && hprocess) {
			*hprocess = NULL;
		}
	}
	return sucess;
}


//Injecte la dll (arg 2) dans le processus (arg 1).
BOOL DLLInject(_In_ HANDLE hProcess, _In_ TCHAR * dllPath, _In_ BOOL is32BitProcess, _Out_ HANDLE *remoteThread) {
	BOOL sucess = TRUE;
	LPVOID dllPathAddressInRemoteMemory = NULL;
	LPVOID loadLibraryAddress = NULL;


	__try {
		_tprintf(_T("%s\n"), dllPath);

		if (!hProcess || !dllPath || !remoteThread) {
			SetLastError(ERROR_INVALID_PARAMETER);
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}
		*remoteThread = INVALID_HANDLE_VALUE;

		//Alloue de l'espace dans hprocess.
		dllPathAddressInRemoteMemory = VirtualAllocEx(hProcess, NULL, _tcslen(dllPath) * sizeof(TCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!dllPathAddressInRemoteMemory) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}

		//Ecrit le path de la dll dans cette espace.
		if (!WriteProcessMemory(hProcess, dllPathAddressInRemoteMemory, dllPath, _tcslen(dllPath) * sizeof(TCHAR), NULL)) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}


		//Renvoie l'adress de la fonction LoadLibraryA qui est commune à tous les processus windows (à condition d'utiliser kernel 32).
		if (is32BitProcess) {
			loadLibraryAddress = NULL;
#ifdef UNICODE
			if (!get32BitAdress(L"kernel32.dll", L"LoadLibraryW", (PDWORD)&loadLibraryAddress)) {
				sucess = FALSE;
				__leave;
			}
#else
			if (!get32BitAdress(L"kernel32.dll", L"LoadLibraryA", (PDWORD)&loadLibraryAddress)) {
				sucess = FALSE;
				__leave;
			}
#endif
		}
		else {
#ifdef UNICODE
			loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryW");
#else
			loadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "LoadLibraryA");
#endif
			if (!loadLibraryAddress) {
				sucess = FALSE;
				__leave;
			}
		}

		//Créé un thread dans hProcess utilisant la fonction LoadLibraryA et prenant comme argument le path de la dll à injecter.
		*remoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddress, dllPathAddressInRemoteMemory, 0, NULL);
		if (!*remoteThread) {
			sucess = FALSE;
			MSDN_FUNC_ERROR();
		}


	}
	__finally {
		if (dllPathAddressInRemoteMemory) {
			//		VirtualFreeEx(hProcess, dllPathAddressInRemoteMemory, _tcslen(dllPath) * sizeof(TCHAR), MEM_RELEASE);
		}
	}
	return sucess;

}

BOOL get32BitAdress(_In_ WCHAR * dllName, _In_ WCHAR * functionName, _Out_ PDWORD adresse32Bit) {
	BOOL sucess = TRUE;
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	WCHAR * path = _32BITGETADRESS;
	WCHAR *commandLine = NULL;
	int i, j;

	TCHAR szMsg[256] = { 0 };
	HANDLE hMapFile = NULL;
	LPCTSTR pBuf = NULL;

	__try {

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!dllName || !functionName || !adresse32Bit) {
			SetLastError(ERROR_INVALID_PARAMETER);
			sucess = FALSE;
			__leave;
		}

		commandLine = (WCHAR*)calloc(sizeof(WCHAR), wcslen(path) + wcslen(dllName) + wcslen(functionName) + 4);
		if (!commandLine) {
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			sucess = FALSE;
			__leave;
		}

		i = 0;
		commandLine[i] = L' ';
		i = 1;
		for (j = 0; dllName[j] != L'\0'; j++) {
			commandLine[i] = dllName[j];
			i++;
		}
		commandLine[i] = L' ';
		i++;
		for (j = 0; functionName[j] != L'\0'; j++) {
			commandLine[i] = functionName[j];
			i++;
		}
		commandLine[i] = L'\0';

		hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 256, _T("functionDll32BitAdress"));
		if (!hMapFile) {
			sucess = FALSE;
			__leave;
		}

		pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 256);
		if (!pBuf) {
			sucess = FALSE;
			__leave;
		}

		CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));

		if (!CreateProcessW(path, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			sucess = FALSE;
			__leave;
		}

		WaitForSingleObject(pi.hProcess, INFINITE);

		for (i = 0; ((char*)pBuf)[i] != L'\0'; i++);
		for (i--; i >= 0; i--) {
			*adresse32Bit *= 10;
			*adresse32Bit += ((char*)pBuf)[i] - L'0';
		}
	}
	__finally {
		if (!sucess) {
			*adresse32Bit = 0;
		}

		if (commandLine) {
			free(commandLine);
		}
		if (pBuf) {
			UnmapViewOfFile(pBuf);
		}
		if (hMapFile) {
			CloseHandle(hMapFile);
		}
		if (pi.dwProcessId) {
			CloseHandle(pi.hProcess);
		}
		if (pi.hThread) {
			CloseHandle(pi.hThread);
		}
	}
	return sucess;
}
