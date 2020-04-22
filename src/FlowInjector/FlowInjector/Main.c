#include "Main.h"

#define NUMBER_OF_DEFAULT_DLL 5


#define DLL64_INSTRUCTION_COUNTER _T("INSTRUCTIONCounter.dll")
#define DLL64_HOOK _T("Hook.dll")
#define DLL64_INJECTOR _T("Injector.dll")
#define DLL64_INSTRUCTION_POINTER_MANAGEMENT _T("Flow.dll")
#define DLL64_DBGHELP _T("../../../Extern/Dll/dbghelp.dll")

void sneakyMain(void);

int _tmain(int argc, TCHAR ** argv) {
	BOOL success = TRUE;
	TCHAR * pathDllToInject[10] = {
		DLL64_INSTRUCTION_COUNTER,
		DLL64_HOOK,
		DLL64_INJECTOR,
		DLL64_INSTRUCTION_POINTER_MANAGEMENT,
		DLL64_DBGHELP
	};
	TCHAR * pathVictim = NULL;
	TCHAR * commandLineVictim = NULL;
	DWORD commandLineVictimLength = 0;
	DWORD pathDllToInjectLength = NUMBER_OF_DEFAULT_DLL;
	DWORD userDllLength = 0;
	BOOL putGuillement;
	DWORD i, j, k;


	__try {

	
		if (argc < 3) {
			success = FALSE;
			__leave;
		}

		userDllLength = _tstoi(argv[1]);

		if (userDllLength > 10 - NUMBER_OF_DEFAULT_DLL) {
			success = FALSE;
			__leave;
		}

		if (userDllLength + 2 > (DWORD)argc) {
			success = FALSE;
			__leave;
		}

		pathDllToInjectLength += userDllLength;
		for (i = 0; i < userDllLength; i++) {
			pathDllToInject[i + NUMBER_OF_DEFAULT_DLL] = argv[i + 2];
		}

		pathVictim = argv[i + 2];

		for (i = userDllLength + NUMBER_OF_DEFAULT_DLL; i < (DWORD)argc; i++) {
			putGuillement = FALSE;
			for (j = 0; argv[i][j] != _T('\0'); j++) {
				if (argv[i][j] == _T(' ')) {
					putGuillement = TRUE;
				}
				commandLineVictimLength++; //basic characters
			}
			if (putGuillement) {
				commandLineVictimLength+=2; //guillement
			}
			commandLineVictimLength++; //final space
		}

		if (commandLineVictimLength != 0 && (commandLineVictim = malloc(commandLineVictimLength * sizeof(TCHAR))) == NULL) {
			success = FALSE;
			__leave;
		}
		
		if (commandLineVictimLength != 0) {
			k = 0;
			for (i = userDllLength + 3; i < (DWORD)argc; i++) {
				putGuillement = FALSE;
				for (j = 0; argv[i][j] != _T('\0'); j++) {
					if (argv[i][j] == _T(' ')) {
						putGuillement = TRUE;
					}
				}

				if (putGuillement) {
					commandLineVictim[k] = _T('\"'); k++;
				}
				for (j = 0; argv[i][j] != _T('\0'); j++) {
					commandLineVictim[k] = argv[i][j]; k++;
				}
				if (putGuillement) {
					commandLineVictim[k] = _T('\"'); k++;
				}
				commandLineVictim[k] = _T(' '); k++;
			}
			commandLineVictim[k - 1] = _T('\0');
		}

		injectEmulatorAndUserDll(pathDllToInject, pathDllToInjectLength, pathVictim, commandLineVictim);
		printf("end of initialisation\n");
	}
	__finally {
		if (commandLineVictim) {
			free(commandLineVictim);
		}
	}

	return success;
}
