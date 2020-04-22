#ifndef _MODRM_H_
#define _MODRM_H_

#define REX_SIGNATURE 4

/**
 * Used internally. See Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2, chapter 2, section 2.2.1, REX Prexixes
 */
typedef union REX_ {
	struct {
		unsigned char B : 1;
		unsigned char X : 1;
		unsigned char R : 1;
		unsigned char W : 1;
		unsigned char Signature : 4; //must me 0100b (4) or it is not a REX
	};
	unsigned char EntireByte;
}REX;

/**
 * Used internally. See Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2, chapter 2, section 2.1.3, ModR/M and SIB Bytes
 */
typedef struct MODRM_ {
	unsigned char Rm : 3;
	unsigned char Reg : 3;
	unsigned char Mod : 2;
}MODRM;

/**
 * Used internally. See Intel® 64 and IA-32 Architectures Software Developer’s Manual Volume 2, chapter 2, section 2.1.3, ModR/M and SIB Bytes
 */
typedef struct SIB_ {
	unsigned char Base : 3;
	unsigned char Index : 3;
	unsigned char Scale : 2;
}SIB;

/**
 * Simplified x64 context. It contains the instruction pointer (RIP), the volatile floating registers (Xmm0-6), the integer registers, and the flag register (EFlags).
 */
typedef struct THREAD_CONTEXT_ {
	DWORD64 Rip;

	DWORD64 Rax;
	DWORD64 Rcx;
	DWORD64 Rdx;
	DWORD64 Rbx;
	DWORD64 Rsp;
	DWORD64 Rbp;
	DWORD64 Rsi;
	DWORD64 Rdi;
	DWORD64 R8;
	DWORD64 R9;
	DWORD64 R10;
	DWORD64 R11;
	DWORD64 R12;
	DWORD64 R13;
	DWORD64 R14;
	DWORD64 R15;

	DWORD64 EFlags; //!< We do not care aboute higher 32 bits, this is why we called it Eflags.

	M128A Xmm0;
	M128A Xmm1;
	M128A Xmm2;
	M128A Xmm3;
	M128A Xmm4;
	M128A Xmm5;
}THREAD_CONTEXT;


/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB void * GetModRMAddress(
	_In_ MODRM modrm,
	_In_ REX rex,
	_In_ const OPCODE * opcode,
	_In_ const THREAD_CONTEXT * context,
	_In_ BYTE instructionLength,
	_In_ BYTE addressSize,
	_Out_ BOOLEAN * isThereMemoryAccess);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetModRMValue(
	_In_ MODRM modrm,
	_In_ REX rex,
	_In_ const OPCODE * opcode,
	_In_ const THREAD_CONTEXT * context,
	_In_ BYTE instructionLength,
	_In_ BYTE operandSize,
	_In_ BYTE addressSize
);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB QWORD getRegisterValue(BYTE id, REX rex, BYTE operandSize, BOOLEAN rexBit, const THREAD_CONTEXT* context);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB QWORD GetSIBValue(SIB sib, MODRM modrm, REX rex, const OPCODE * Opcode, const THREAD_CONTEXT * context);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BYTE GetModRmSize(const OPCODE * Opcode);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN GetRmString(MODRM modrm, REX rex, const OPCODE * Opcode, void * rip, BYTE INSTRUCTIONSize, BYTE operandSize, BOOLEAN addressSize, char * str);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN GetRegString(MODRM modrm, REX rex, BYTE operandSize, char * str);

/**
 * Used internally.
 */
INSTRUCTIONCOUNTER_LIB BOOLEAN getSIBString(SIB sib, MODRM modrm, REX rex, const OPCODE * Opcode, BOOLEAN addressSize, char * str);


#endif // !_MODRM_H_
