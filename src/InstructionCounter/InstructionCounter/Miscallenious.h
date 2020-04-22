#ifndef _MISCALLENIOUS_H_
#define _MISCALLENIOUS_H_

/**
 * Used internally
 */
inline void memCopy(char* dest, const char* source, int size) {
	int i;
	for (i = 0; i < size; i++) {
		dest[i] = source[i];
	}
}

/**
 * Used internally
 */
inline QWORD getStrLength(const char* const str) {
	QWORD i;
	for (i = 0; str[i] != '\0'; i++);
	return i;
}

/**
 * Used internally
 */
BOOL qwordToHexStr(QWORD input, char str[17]);

#endif // !_MISCALLENIOUS_H_
