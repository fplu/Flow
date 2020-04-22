#ifndef _JMP_CALL_RET_H_
#define _JMP_CALL_RET_H_




#define JMP_REL8 0xEB
#define JMP_REL8_SIZE 2
#define JMP_REL32 0xE9
#define JMP_REL32_SIZE 5
#define JMP_MODRM_4 0xFF

#define CALL_REL32 0XE8
#define CALL_MODRM_2 0xFF

/**
 * Say if the instruction which start as opcode is a Jmp instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Jmp instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsJmp(const OPCODE * opcode);

/**
 * Say if the instruction which start as opcode is a Call instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Call instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsCall(const OPCODE * opcode);

/**
 * Return the destination of a Jmp instruction. It fails if the opcode pass as paramter are not from a jmp or the context pass as paramater is NULL even if it was needed
 *
 *
 * @param[in] jmpOpcode start of the instruction (do not include prefix).
 * @param[in] rex rex (see intel documentation for more information) prefix of the instruction, 0 if the instruction has no rex prefix. This value can be found using GetInstruction.
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @param[in] context context of the thread which execute the instruction. It could be NULL if the result of the instruction do not depend on general purpose register.
 * @param[in] addressSize address size of the instruction see : "address-size override prefix" for more information. This value can be found using GetInstruction. It is 8 most of the time.
 * @return the destination of the Jmp or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetJmpDest(const OPCODE * jmpOpcode, REX rex, const OPCODE * rip, BYTE instructionLength, THREAD_CONTEXT * context, BOOLEAN addressSize);

/**
 * Return the destination of a Call instruction. It fails if the opcode pass as paramter are not from a call or the context pass as paramater is NULL even if it was needed.
 *
 *
 * @param[in] callOpcode start of the instruction (do not include prefix).
 * @param[in] rex rex (see intel documentation for more information) prefix of the instruction, 0 if the instruction has no rex prefix. This value can be found using GetInstruction.
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @param[in] context context of the thread which execute the instruction. It could be NULL if the result of the instruction do not depend on general purpose register.
 * @param[in] addressSize address size of the instruction see : "address-size override prefix" for more information. This value can be found using GetInstruction. It is 8 most of the time.
 * @return the destination of the Call or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetCallDest(const OPCODE * callOpcode, REX rex, const OPCODE * rip, BYTE instructionLength, THREAD_CONTEXT * context, BOOLEAN addressSize);

/**
 * Return the destination of a Call rel32 instruction. It fails if the opcode pass as paramter are not from a Call rel32.
 *
 *
 * @param[in] callOpcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the Call or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetCallRel32Dest(const OPCODE * callOpcode, const OPCODE * rip, BYTE instructionLength);

/**
 * Return the destination of a Jmp rel32 instruction. It fails if the opcode pass as paramter are not from a Jmp rel 32.
 *
 *
 * @param[in] jmpOpcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the Jmp or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetJmpRel32Dest(const OPCODE * jmpOpcode, const OPCODE * rip, BYTE instructionLength);


/**
 * Return the destination of a Jmp rel8 instruction. It fail if the opcode pass as paramter are not from a Jmp rel 8.
 *
 *
 * @param[in] jmpOpcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the Jmp or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetJmpRel8Dest(const OPCODE * jmpOpcode, const OPCODE * rip, BYTE instructionLength);


#endif // !_JMP_CALL_RET_H_
