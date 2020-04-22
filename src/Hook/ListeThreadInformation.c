#include "Hook.h"

DWORD tlsIndex = TLS_OUT_OF_INDEXES;
PListeThreadInformation g_listeThreadInformation = NULL;


void saveCallAddress(_In_ PVOID callAddress) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	PVOID threadId = (PVOID)(QWORD)GetCurrentThreadId();

	__try {
		if (TlsGetValue(tlsIndex) == NULL) {
			if ((currentNode = calloc(1, sizeof(ListeThreadInformation))) == NULL || !TlsSetValue(tlsIndex, currentNode)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = TlsGetValue(tlsIndex);
		currentNode->callAddress = callAddress;
	}
	__finally {

	}
}

PVOID getCallAddress(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	PVOID callAddress = NULL;

	__try {
		currentNode = TlsGetValue(tlsIndex);
		callAddress = currentNode->callAddress;
	}
	__finally {

	}
	return callAddress;
}


void saveRetAddress(_In_ PVOID retAddress) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	PVOID threadId = (PVOID)(QWORD)GetCurrentThreadId();

	__try {
		if (TlsGetValue(tlsIndex) == NULL) {
			if ((currentNode = calloc(1, sizeof(ListeThreadInformation))) == NULL || !TlsSetValue(tlsIndex, currentNode)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = TlsGetValue(tlsIndex);
		currentNode->retAddress = retAddress;
	}
	__finally {

	}
}

PVOID getRetAddress(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	PVOID retAddress = NULL;
	__try {
		currentNode = TlsGetValue(tlsIndex);
		retAddress = currentNode->retAddress;
	}
	__finally {

	}
	return retAddress;
}


void savePrecallback(_In_ PVOID precallback) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;

	__try {
		if (TlsGetValue(tlsIndex) == NULL) {
			if ((currentNode = calloc(1, sizeof(ListeThreadInformation))) == NULL || !TlsSetValue(tlsIndex, currentNode)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = TlsGetValue(tlsIndex);
		currentNode->precallback = precallback;
	}
	__finally {

	}
}

PVOID getPrecallback(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	PVOID precallback = NULL;
	__try {
		currentNode = TlsGetValue(tlsIndex);
		precallback = currentNode->precallback;
	}
	__finally {

	}
	return precallback;
}


void savePostcallback(_In_ PVOID postcallback) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;

	__try {
		if (TlsGetValue(tlsIndex) == NULL) {
			if ((currentNode = calloc(1, sizeof(ListeThreadInformation))) == NULL || !TlsSetValue(tlsIndex, currentNode)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = TlsGetValue(tlsIndex);
		currentNode->postcallback = postcallback;

	}
	__finally {

	}
}

PVOID getPostcallback(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	PVOID postcallback = NULL;
	__try {
		currentNode = TlsGetValue(tlsIndex);
		postcallback = currentNode->postcallback;
	}
	__finally {

	}
	return postcallback;
}




void saveOnlyPrecallback(_In_ BOOL onlyPrecallback) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;

	__try {
		if (TlsGetValue(tlsIndex) == NULL) {
			if ((currentNode = calloc(1, sizeof(ListeThreadInformation))) == NULL || !TlsSetValue(tlsIndex, currentNode)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = TlsGetValue(tlsIndex);
		currentNode->onlyPrecallback = onlyPrecallback;
	}
	__finally {

	}
}

BOOL getOnlyPrecallback(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	BOOL onlyPrecallback = FALSE;
	__try {
		currentNode = TlsGetValue(tlsIndex);
		onlyPrecallback = currentNode->onlyPrecallback;
	}
	__finally {

	}
	return onlyPrecallback;
}


CONTEXT* getRegister(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	CONTEXT* context = NULL;
	__try {
		currentNode = TlsGetValue(tlsIndex);
		context = &(currentNode->context);
	}
	__finally {

	}
	return context;
}

void saveStack(_In_ QWORD stackValue, _In_ BYTE stackIndex) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;

	__try {
		if (TlsGetValue(tlsIndex) == NULL) {
			if ((currentNode = calloc(1, sizeof(ListeThreadInformation))) == NULL || !TlsSetValue(tlsIndex, currentNode)) {
				success = FALSE;
				__leave;
			}
		}
		currentNode = TlsGetValue(tlsIndex);
		currentNode->stack[stackIndex] = stackValue;

	}
	__finally {

	}
}

QWORD* getStack(void) {
	BOOL success = TRUE;
	PListeThreadInformation currentNode = NULL;
	QWORD* stack = NULL;
	__try {
		currentNode = TlsGetValue(tlsIndex);
		stack = currentNode->stack;
	}
	__finally {

	}
	return stack;
}