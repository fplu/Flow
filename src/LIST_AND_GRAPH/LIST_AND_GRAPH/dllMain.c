#include <Windows.h>


BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {
	Beep(1000, 1000);

	return TRUE;
}
