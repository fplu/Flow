#include "Hook.h"

BOOL pushListeHook(_Out_ PListHook * head, _In_ size_t sizeHook) {
	BOOL success = TRUE;
	int indexStartHook;

	__try {
		if (!head) {
			SetLastError(ERROR_INVALID_PARAMETER);
			success = FALSE;
			__leave;
		}

		if (*head) {
			indexStartHook = (int)(*head)->indexStartHook + (int)(*head)->sizeHook;
		}
		else {
			indexStartHook = 0;
		}


		if (!pushNodeLIST_BASIC(NULL, sizeof(ListHook), (PLIST_BASIC*)head)) {
			success = FALSE;
			__leave;
		}

		(*head)->indexStartHook = indexStartHook;
		(*head)->sizeHook = sizeHook;
	}
	__finally {

	}

	return success;
}


