#include "MainInstructionCounter.h"

BOOLEAN IsBranch(const OPCODE * opcode) {
	return IsRet(opcode) || IsCall(opcode) || IsJmp(opcode) || IsJccOrLoop(opcode);
}


OPCODE* GetBranchDest(const OPCODE * branchOpcode, REX rex, const OPCODE* rip, BYTE instructionLength, THREAD_CONTEXT * context, BYTE addressSize, BOOLEAN hasOperandSizePrefix) {
	if (IsRet(branchOpcode)) {
		if (context) {
			return (OPCODE*)GetRetDest(branchOpcode, rip, instructionLength, (void*)context->Rsp, hasOperandSizePrefix);
		}
		else {
			return NULL;
		}
	}
	if (IsCall(branchOpcode)) {
		return (OPCODE*)GetCallDest(branchOpcode, rex, rip, instructionLength, context, addressSize);
	}
	if (IsJmp(branchOpcode)) {
		return (OPCODE*)GetJmpDest(branchOpcode, rex, rip, instructionLength, context, addressSize);
	}
	if (IsJccOrLoop(branchOpcode)) {
		return (OPCODE*)GetJccOrLoopDest(branchOpcode, rip, instructionLength);
	}
	return NULL;
}

