#ifndef _BRANCH_H_
#define _BRANCH_H_

/**
 * Say if the instruction which start as opcode is a branch instruction (Call, Ret, Jmp, Loop or Jcc).
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a branch instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsBranch(const OPCODE * opcode);

/**
 * Return the destination of a branch instruction (Call, Ret, Jmp, Loop or Jcc), considering that the condition of the branch is always TRUE. It fails if the opcode pass as paramter are not from a branch or the context pass as paramater is NULL even if it was needed.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefix).
 * @param[in] rex rex (see intel documentation for more information) prefix of the instruction, 0 if the instruction has no rex prefix. This value can be found using GetInstruction.
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @param[in] context context of the thread which execute the instruction. It could be NULL if the result of the instruction do not depend on general purpose register.
 * @param[in] addressSize address size of the instruction see : "address-size override prefix" for more information. This value can be found using GetInstruction. It is 8 most of the time.
 * @param[in] hasOperandSizePrefix operand size of the instruction see : "operand-size override prefix" for more information. This value can be found using GetInstruction. It is FALSE most of the time.
 * @return the destination of the branch or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB OPCODE* GetBranchDest(const OPCODE * branchOpcode, REX rex, const OPCODE* rip, BYTE instructionLength, THREAD_CONTEXT * context, BYTE addressSize, BOOLEAN hasOperandSizePrefix);


#endif // !_BRANCH_H_
