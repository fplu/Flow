#include "MainINSTRUCTIONCounter.h"

BOOL qwordToHexStr(QWORD input, char str[17]) {
	BOOL success = TRUE;
	int i;
	//int strLength;

	__try {
		if (str == NULL) {
			success = FALSE;
			__leave;
		}

		str[16] = '\0';


		i = 15;
		for (; input > 0; input /= 16) {
			if (input % 16 < 10) {
				str[i--] = '0' + input % 16;
			}
			else {
				str[i--] = 'A' + input % 16 - 10;
			}
		}
		for (; i >= 0; i--) {
			str[i] = '0';
		}

	}
	__finally {

	}
	return success;
}


