#include "MainINSTRUCTIONCounter.h"


const OPCODE g_OpcodeJccRel8[] = { 0x77,0x73,0x72,0x76,0x72,0xE3,0xE3,0x74,0x7F,0x7D,0x7C,0x7E,0x76,0x72,0x73,0x77,0x73,0x75,0x7E,0x7C,0x7D,0x7F,0x71,0x7B,0x79,0x75,0x70,0x7A,0x7A,0x7B,0x78,0x74 };

//first Opcode 0x0F
const OPCODE g_OpcodeJccRel32[] = { 0x87,0x83,0x82,0x86,0x82,0x84,0x84,0x8F,0x8D,0x8C,0x8E,0x86,0x82,0x83,0x85,0x8E,0x8C,0x8D,0x8F,0x81,0x8B,0x89,0x85,0x80,0x8A,0x8A,0x8B,0x88,0x84 };

const OPCODE LoopINSTRUCTIONs[] = {
	LOOP_REL8,
	LOOPE_REL8,
	LOOPNE_REL8
};



BOOLEAN IsJccRel8(const OPCODE * opcode) {
	DWORD i;
	for (i = 0; i < sizeof(g_OpcodeJccRel8); i++) {
		if (opcode[0] == g_OpcodeJccRel8[i]) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOLEAN IsJccRel32(const OPCODE * opcode) {
	DWORD i;
	if (opcode[0] != 0x0F) {
		return FALSE;
	}
	for (i = 0; i < sizeof(g_OpcodeJccRel32); i++) {
		if (opcode[1] == g_OpcodeJccRel32[i]) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOLEAN IsLoop(const OPCODE * opcode) {
	DWORD i;
	
	for (i = 0; i < sizeof(LoopINSTRUCTIONs); i++) {
		if (opcode[0] == LoopINSTRUCTIONs[i]) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOLEAN IsJcc(const OPCODE * opcode) {
	if (IsJccRel8(opcode)) {
		return TRUE;
	}
	if (IsJccRel32(opcode)) {
		return TRUE;
	}
	return FALSE;
}


BOOLEAN IsJccOrLoop(const OPCODE * opcode) {
	if (IsJccRel8(opcode)) {
		return TRUE;
	}
	if (IsJccRel32(opcode)) {
		return TRUE;
	}
	if (IsLoop(opcode)) {
		return TRUE;
	}
	return FALSE;
}

QWORD GetJccOrLoopDest(const OPCODE * opcode, const OPCODE * rip, BYTE instructionLength) {
	if (IsJccRel8(opcode)) {
		return GetJcc8Dest(opcode, rip, instructionLength);
	}
	if (IsJccRel32(opcode)) {
		return GetJcc32Dest(opcode, rip, instructionLength);
	}
	if (IsLoop(opcode)) {
		return GetLoopDest(opcode, rip, instructionLength);
	}
	return (QWORD)NULL;
}

QWORD GetJcc8Dest(const OPCODE * jccOpcode, const OPCODE * rip, BYTE instructionLength) {
	return (QWORD)((BYTE*)rip + *(char*)(jccOpcode + 1) + instructionLength);
}

QWORD GetJcc32Dest(const OPCODE * jccOpcode, const OPCODE * rip, BYTE instructionLength) {
	return (QWORD)((BYTE*)rip + *(int*)(jccOpcode + 2) + instructionLength);
}

QWORD GetLoopDest(const OPCODE * jccOpcode, const OPCODE * rip, BYTE instructionLength) {
	return (QWORD)((BYTE*)rip + *(char*)(jccOpcode + 1) + instructionLength);
}

BOOLEAN IsJccOrLoopActive(OPCODE * opcode, DWORD eflags, QWORD rcx, BOOLEAN hasAddressSizePrefix) {



	if (opcode[0] == LOOP_REL8 && hasAddressSizePrefix) {
		return ((DWORD)rcx) != 0;
	}
	else if (opcode[0] == LOOP_REL8 && !hasAddressSizePrefix) {
		return rcx != 0;
	}
	if (opcode[0] == LOOPE_REL8 && hasAddressSizePrefix) {
		return ((DWORD)rcx) != 0 && (eflags & ZF) != 0;
	}
	else if (opcode[0] == LOOPE_REL8 && !hasAddressSizePrefix) {
		return rcx != 0 && (eflags & ZF) != 0;
	}
	if (opcode[0] == LOOPNE_REL8 && hasAddressSizePrefix) {
		return ((DWORD)rcx) != 0 && (eflags & ZF) == 0;
	}
	else if (opcode[0] == LOOPNE_REL8 && !hasAddressSizePrefix) {
		return rcx != 0 && (eflags & ZF) == 0;
	}
	else if (opcode[0] == JA_REL8) {
		return (eflags & CF) == 0 && (eflags & ZF) == 0;
	}
	else if (opcode[0] == JAE_REL8) {
		return (eflags & CF) == 0;
	}
	else if (opcode[0] == JB_REL8) {
		return (eflags & CF) != 0;
	}
	else if (opcode[0] == JBE_REL8) {
		return (eflags & CF) != 0 || (eflags & ZF) != 0;
	}
	else if (opcode[0] == JC_REL8) {
		return (eflags & CF) != 0;
	}
	else if (opcode[0] == JECXZ_REL8 && hasAddressSizePrefix) {
		return ((int)rcx) == 0;
	}
	else if (opcode[0] == JRCXZ_REL8 && !hasAddressSizePrefix) {
		return rcx == 0;
	}
	else if (opcode[0] == JE_REL8) {
		return (eflags & ZF) != 0;
	}
	else if (opcode[0] == JG_REL8) {
		return (eflags & ZF) == 0 && (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[0] == JGE_REL8) {
		return (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[0] == JL_REL8) {
		return (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[0] == JLE_REL8) {
		return (eflags & ZF) != 0 || (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[0] == JNA_REL8) {
		return (eflags & CF) != 0 || (eflags & ZF) != 0;
	}
	else if (opcode[0] == JNAE_REL8) {
		return (eflags & CF) != 0;
	}
	else if (opcode[0] == JNB_REL8) {
		return (eflags & CF) == 0;
	}
	else if (opcode[0] == JNBE_REL8) {
		return (eflags & CF) == 0 && (eflags & ZF) == 0;
	}
	else if (opcode[0] == JNC_REL8) {
		return (eflags & CF) == 0;
	}
	else if (opcode[0] == JNE_REL8) {
		return (eflags & ZF) == 0;
	}
	else if (opcode[0] == JNG_REL8) {
		return (eflags & ZF) != 0 || (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[0] == JNGE_REL8) {
		return (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[0] == JNL_REL8) {
		return (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[0] == JNLE_REL8) {
		return (eflags & ZF) == 0 && (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[0] == JNO_REL8) {
		return (eflags & OF) == 0;
	}
	else if (opcode[0] == JNP_REL8) {
		return (eflags & PF) == 0;
	}
	else if (opcode[0] == JNS_REL8) {
		return (eflags & SF) == 0;
	}
	else if (opcode[0] == JNZ_REL8) {
		return (eflags & ZF) == 0;
	}
	else if (opcode[0] == JO_REL8) {
		return (eflags & OF) != 0;
	}
	else if (opcode[0] == JP_REL8) {
		return (eflags & PF) != 0;
	}
	else if (opcode[0] == JPE_REL8) {
		return (eflags & PF) != 0;
	}
	else if (opcode[0] == JPO_REL8) {
		return (eflags & PF) == 0;
	}
	else if (opcode[0] == JS_REL8) {
		return (eflags & SF) != 0;
	}
	else if (opcode[0] == JZ_REL8) {
		return (eflags & ZF) != 0;
	}
	else if (opcode[0] != 0x0F) {
		return FALSE;
	}
	else if (opcode[1] == JA_REL32) {
		return ((eflags & CF) == 0) && ((eflags & ZF) == 0);
	}
	else if (opcode[1] == JAE_REL32) {
		return (eflags & CF) == 0;
	}
	else if (opcode[1] == JB_REL32) {
		return (eflags & CF) != 0;
	}
	else if (opcode[1] == JBE_REL32) {
		return (eflags & CF) != 0 || (eflags & ZF) != 0;
	}
	else if (opcode[1] == JC_REL32) {
		return (eflags & CF) != 0;
	}
	else if (opcode[1] == JE_REL32) {
		return (eflags & ZF) != 0;
	}
	else if (opcode[1] == JG_REL32) {
		return (eflags & ZF) == 0 && (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[1] == JGE_REL32) {
		return (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[1] == JL_REL32) {
		return (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[1] == JLE_REL32) {
		return (eflags & ZF) != 0 || (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[1] == JNA_REL32) {
		return (eflags & CF) != 0 || (eflags & ZF) != 0;
	}
	else if (opcode[1] == JNAE_REL32) {
		return (eflags & CF) != 0;
	}
	else if (opcode[1] == JNB_REL32) {
		return (eflags & CF) == 0;
	}
	else if (opcode[1] == JNBE_REL32) {
		return (eflags & CF) == 0 && (eflags & ZF) == 0;
	}
	else if (opcode[1] == JNC_REL32) {
		return (eflags & CF) == 0;
	}
	else if (opcode[1] == JNE_REL32) {
		return (eflags & ZF) == 0;
	}
	else if (opcode[1] == JNG_REL32) {
		return (eflags & ZF) != 0 || (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[1] == JNGE_REL32) {
		return (((eflags & SF) != 0) != ((eflags & OF) != 0));
	}
	else if (opcode[1] == JNL_REL32) {
		return (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[1] == JNLE_REL32) {
		return (eflags & ZF) == 0 && (((eflags & SF) != 0) == ((eflags & OF) != 0));
	}
	else if (opcode[1] == JNO_REL32) {
		return (eflags & OF) == 0;
	}
	else if (opcode[1] == JNP_REL32) {
		return (eflags & PF) == 0;
	}
	else if (opcode[1] == JNS_REL32) {
		return (eflags & SF) == 0;
	}
	else if (opcode[1] == JNZ_REL32) {
		return (eflags & ZF) == 0;
	}
	else if (opcode[1] == JO_REL32) {
		return (eflags & OF) != 0;
	}
	else if (opcode[1] == JP_REL32) {
		return (eflags & PF) != 0;
	}
	else if (opcode[1] == JPE_REL32) {
		return (eflags & PF) != 0;
	}
	else if (opcode[1] == JPO_REL32) {
		return (eflags & PF) == 0;
	}
	else if (opcode[1] == JS_REL32) {
		return (eflags & SF) != 0;
	}
	else if (opcode[1] == JZ_REL32) {
		return (eflags & ZF) != 0;
	}
	return FALSE;
}

