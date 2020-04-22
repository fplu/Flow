#ifndef _RET_H_
#define _RET_H_

#define RET 0xC3
#define RET_IMM16 0xC2
#define IRET 0xCF


/**
 * Say if the instruction which start as opcode is a Ret instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Ret instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsRet(const OPCODE * opcode);

/**
 * Return the destination of a Ret instruction. It fails if the opcode pass as paramter are not from a ret
 *
 * @param[in] retOpcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @param[in] rsp value of rsp before the instruction is executed.
 * @param[in] hasOperandSizePrefix operand size of the instruction see : "operand-size override prefix" for more information. This value can be found using GetInstruction. It is FALSE most of the time.
 * @return the destination of the Ret or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetRetDest(const OPCODE * retOpcode, const void * rip, BYTE instructionLength, void * rsp, BOOLEAN hasOperandSizePrefix);


#endif // !_RET_H_
