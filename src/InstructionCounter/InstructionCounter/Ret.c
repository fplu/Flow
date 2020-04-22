#include "MainINSTRUCTIONCounter.h"

//do not handle far ret
const OPCODE g_OpcodeRet[] = {
	RET_NEAR,
	RET_NEAR_POP_IMM6,
	IRET
};


BOOLEAN IsRet(const OPCODE * opcode) {
	DWORD i;
	for (i = 0; i < sizeof(g_OpcodeRet); i++) {
		if (opcode[0] == g_OpcodeRet[i]) {
			return TRUE;
		}
	}
	return FALSE;
}


QWORD GetRetDest(const OPCODE * retOpcode, const void * rip, BYTE instructionLength, void * rsp, BOOLEAN hasOperandSizePrefix) {
	QWORD res = (QWORD)NULL;

	if (hasOperandSizePrefix) {
		res = (QWORD)*((DWORD*)rsp);
	}
	else {
		res = (QWORD)*((QWORD*)rsp);
	}
	return res;
}
