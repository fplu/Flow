#include "MainINSTRUCTIONCounter.h"

//CALL AND JUMP FAR NOT SUPPORTED
const OPCODE g_OpcodeJmp[] = {
	JMP_REL8,
	JMP_REL32,
	JMP_MODRM_4
};

const OPCODE g_OpcodeCall[] = {
	CALL_REL32,
	CALL_MODRM_2,
};


BOOLEAN IsJmp(const OPCODE * opcode) {
	DWORD i;
	for (i = 0; i < sizeof(g_OpcodeJmp); i++) {
		if (opcode[0] == g_OpcodeJmp[i]) {
			if (opcode[0] == JMP_MODRM_4 && (*(MODRM*)(opcode + 1)).Reg != 4) {
				continue;
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOLEAN IsCall(const OPCODE * opcode) {
	DWORD i;
	for (i = 0; i < sizeof(g_OpcodeCall); i++) {
		if (opcode[0] == g_OpcodeCall[i]) {
			if (opcode[0] == CALL_MODRM_2 && (*(MODRM*)(opcode + 1)).Reg != 2) {
				continue;
			}
			return TRUE;
		}
	}
	return FALSE;
}

QWORD GetJmpDest(const OPCODE * jmpOpcode, REX rex, const OPCODE * rip, BYTE instructionLength, THREAD_CONTEXT * context, BOOLEAN addressSize) {
	if (jmpOpcode[0] == JMP_REL8) {
		return  GetJmpRel8Dest(jmpOpcode, rip, instructionLength);
	}
	else if (jmpOpcode[0] == JMP_REL32) {
		return  GetJmpRel32Dest(jmpOpcode, rip, instructionLength);
	}
	else if (jmpOpcode[0] == JMP_MODRM_4 && (*(MODRM*)(jmpOpcode + 1)).Reg == 4) {
		if (context == NULL) {
			return (QWORD)NULL;
		}
		return (QWORD) GetModRMValue(*(MODRM*)(jmpOpcode + 1), rex, jmpOpcode + 2, context, instructionLength, 8, addressSize);
	}
	return (QWORD)NULL;
}

QWORD GetJmpRel8Dest(const OPCODE * jmpOpcode, const OPCODE * rip, BYTE instructionLength) {
	return (QWORD)((BYTE*)rip + *(char*)(jmpOpcode + 1) + instructionLength);
}

QWORD GetJmpRel32Dest(const OPCODE * jmpOpcode, const OPCODE * rip, BYTE instructionLength) {
	return (QWORD)((BYTE*)rip + *(int*)(jmpOpcode + 1) + instructionLength);
}

QWORD GetCallDest(const OPCODE * callOpcode, REX rex, const OPCODE * rip, BYTE instructionLength, THREAD_CONTEXT * context, BOOLEAN addressSize) {
	if (callOpcode[0] == CALL_REL32) {
		return  GetCallRel32Dest(callOpcode, rip, instructionLength);
	}
	else if (callOpcode[0] == CALL_MODRM_2 && (*(MODRM*)(callOpcode + 1)).Reg == 2) {
		if (context == NULL) {
			return (QWORD)NULL;
		}
		return (QWORD)GetModRMValue(*(MODRM*)(callOpcode + 1), rex, callOpcode + 2, context, instructionLength, 8, addressSize);
	}
	return (QWORD)NULL;
}

QWORD GetCallRel32Dest(const OPCODE * callOpcode, const OPCODE * rip, BYTE instructionLength) {
	return (QWORD)((BYTE*)rip + *(int*)(callOpcode + 1) + instructionLength);
}