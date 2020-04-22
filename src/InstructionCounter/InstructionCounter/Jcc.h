#ifndef _JCC_H_
#define _JCC_H_

#define CF 0x01
#define PF 0x04
#define AF 0x10
#define ZF 0x40
#define SF 0x80
#define TF 0x100
#define IF 0x200
#define DF 0x400
#define OF 0x800
#define IOPL 0x3000
#define NT 0x4000
#define RF 0x10000
#define VM 0x20000
#define AC 0x40000
#define VIF 0x80000
#define VIP 0x100000
#define ID 0x200000

#define LOOP_REL8 0xE2
#define LOOPE_REL8 0xE1
#define LOOPNE_REL8 0xE0


#define JA_REL8 0x77
#define JAE_REL8 0x73
#define JB_REL8 0x72
#define JBE_REL8 0x76
#define JC_REL8 0x72
#define JCXZ_REL8 0xE3
#define JECXZ_REL8 0xE3
#define JRCXZ_REL8 0xE3
#define JE_REL8 0x74
#define JG_REL8 0x7F
#define JGE_REL8 0x7D
#define JL_REL8 0x7C
#define JLE_REL8 0x7E
#define JNA_REL8 0x76
#define JNAE_REL8 0x72
#define JNB_REL8 0x73
#define JNBE_REL8 0x77
#define JNC_REL8 0x73
#define JNE_REL8 0x75
#define JNG_REL8 0x7E
#define JNGE_REL8 0x7C
#define JNL_REL8 0x7D
#define JNLE_REL8 0x7F
#define JNO_REL8 0x71
#define JNP_REL8 0x7B
#define JNS_REL8 0x79
#define JNZ_REL8 0x75
#define JO_REL8 0x70
#define JP_REL8 0x7A
#define JPE_REL8 0x7A
#define JPO_REL8 0x7B
#define JS_REL8 0x78
#define JZ_REL8 0x74

#define JA_REL32 0x87
#define JAE_REL32 0x83
#define JB_REL32 0x82
#define JBE_REL32 0x86
#define JC_REL32 0x82
#define JE_REL32 0x84
#define JZ_REL32 0x84
#define JG_REL32 0x8F
#define JGE_REL32 0x8D
#define JL_REL32 0x8C
#define JLE_REL32 0x8E
#define JNA_REL32 0x86
#define JNAE_REL32 0x82
#define JNB_REL32 0x83
#define JNBE_REL32 0x87
#define JNC_REL32 0x83
#define JNE_REL32 0x85
#define JNG_REL32 0x8E
#define JNGE_REL32 0x8C
#define JNL_REL32 0x8D
#define JNLE_REL32 0x8F
#define JNO_REL32 0x81
#define JNP_REL32 0x8B
#define JNS_REL32 0x89
#define JNZ_REL32 0x85
#define JO_REL32 0x80
#define JP_REL32 0x8A
#define JPE_REL32 0x8A
#define JPO_REL32 0x8B
#define JS_REL32 0x88
#define JZ_REL32 0x84


typedef BOOLEAN(*jccTest)(OPCODE * Opcode, DWORD eflags);

/**
 * Say if the instruction which start as opcode is a Jcc rel8 instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Jcc rel8  instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsJccRel8(const OPCODE * opcode);

/**
 * Say if the instruction which start as opcode is a Jcc rel32 instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Jcc rel32 instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsJccRel32(const OPCODE * opcode);

/**
 * Say if the instruction which start as opcode is a Jcc instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Jcc instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsJcc(const OPCODE * opcode);

/**
 * Say if the instruction which start as opcode is a Loop instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Loop instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsLoop(const OPCODE * opcode);


/**
 * Say if the instruction which start as opcode is a Jcc or a Loop instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefixe).
 * @return TRUE if it is a Jcc or a Loop instruction otherwise it returns FALSE.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsJccOrLoop(const OPCODE * opcode);

/**
 * Return the destination of a Jcc or a Loop instruction, considering that the condition of the branch is always TRUE. It fails if the opcode pass as paramter are not from a Jcc or a Loop instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the branch or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetJccOrLoopDest(const OPCODE * opcode, const OPCODE * rip, BYTE instructionLength);

/**
 * Return the destination of a Jcc rel8 instruction, considering that the condition of the branch is always TRUE. It fails if the opcode pass as paramter are not from a Jcc rel 8 instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the branch or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetJcc8Dest(const OPCODE * jccOpcode, const OPCODE * rip, BYTE instructionLength);

/**
 * Return the destination of a Jcc rel32 instruction, considering that the condition of the branch is always TRUE. It fails if the opcode pass as paramter are not from a Jcc rel 32 instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix. 
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the branch or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetJcc32Dest(const OPCODE * jccOpcode, const OPCODE * rip, BYTE instructionLength);

/**
 * Return the destination of a Loop instruction, considering that the condition of the branch is always TRUE. It fails if the opcode pass as paramter are not from a Loop instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefix).
 * @param[in] rip value of rip when the instruction is executed. In other word, it is the start of the instruction which include prefix.
 * @param[in] instructionLength size of the instruction (include prefixe).
 * @return the destination of the branch or NULL if it fails. Be careful, sometimes a branch could jump to the NULL address.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetLoopDest(const OPCODE * jccOpcode, const OPCODE * rip, BYTE instructionLength);


/**
 * Return the state of a Jcc or a Loop instruction.
 *
 *
 * @param[in] opcode start of the instruction (do not include prefix).
 * @param[in] eflags value of eflags before the instruction is executed.
 * @param[in] rcx value of rcx before the instruction is executed.
 * @param[in] addressSize address size of the instruction see : "address-size override prefix" for more information. This value can be found using GetInstruction. It is 8 most of the time.
 * @return TRUE if the branch is taken. FALSE otherwise.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN IsJccOrLoopActive(OPCODE * opcode, DWORD eflags, QWORD rcx, BOOLEAN hasAddressSizePrefix);

#endif // !_JCC_H_
