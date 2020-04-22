#ifndef _MEMORY_ACCESS_H_
#define _MEMORY_ACCESS_H_

#define MEMORY_ACCESS_READ 0x1
#define MEMORY_ACCESS_WRITE 0x2

/*
	Operand used to perform the memory access. They are those which contains the address.
*/
typedef enum MEMORY_ACCESS_OPERAND_e {
	OPERAND_NO_ACCESS = 0,			//!< Default value when there is no memory access.
	OPERAND_RDI_AND_RSI_ACCESS,		//!< Both RDI and RSI are used to perform two different memory access. RDI as write (D for dest), RSI as read (S for source). It is used in pair with TYPE_READ_WRITE_ACCESS
	OPERAND_RSI_ACCESS,				//!< RSI is used to perform a memory access.
	OPERAND_RDI_ACCESS,				//!< RDI is used to perform a memory access.
	OPERAND_MODRM_ACCESS			//!< the ModRM is used to perform a memory access. When the ModRM use RDI or RSI, it is still set to this value.
}MEMORY_ACCESS_OPERAND;

typedef enum MEMORY_ACCESS_TYPE_e {
	TYPE_NO_ACCESS = 0,			//!< Default value when there is no memory access.
	TYPE_READ_ACCESS,			//!< There is a reading access.
	TYPE_WRITE_ACCESS,			//!< There is a writing access.
	TYPE_EXECUTE_ACCESS,		//!< There is a executing access.
	TYPE_READ_WRITE_ACCESS		//!< There are a reading and a writing access. It is used in pair with OPERAND_RDI_AND_RSI_ACCESS.
}MEMORY_ACCESS_TYPE;

//See GetInstruction to get this struct
typedef struct MEMORY_ACCESS_INFORMATION_s {
	MEMORY_ACCESS_OPERAND Operand;	//!< Operand used for the memory access. It indicates where the address is located.
	MEMORY_ACCESS_TYPE Type;		//!< Type of the memory access. It indicates the type of access which is performed.
	DWORD Size;						//!< Size in BIT of the access. I repeat, in BIT.
}MEMORY_ACCESS_INFORMATION;

//See GetMemoryAccessFromInstructionRead to get this struct
typedef struct MEMORY_ACCESS_s {
	void * BaseAddress;				//!< Base address of the memory access
	DWORD Size;						//!< Size in BIT of the access. I repeat, in BIT.
	void * BaseAddressBis;			//!< Base address of the second memory access if two. It is used when TYPE_READ_WRITE_ACCESS is the access type. It contains the reading access.
	BOOL IsThereBaseAddressBis;		//!< Size in BIT of the second memory access. I repeat, in BIT.
}MEMORY_ACCESS;

/**
 * Used internally
 */
BOOL GetInstructionMemoryAccess(
	_In_ MEMORY_ACCESS_INFORMATION memoryAccessInformation,
	_In_ const OPCODE * opcodeAfterPrefix,
	_In_ MODRM modrm,
	_In_ REX rex,
	_In_ const OPCODE * opcodeAfterModRm,
	_In_ const THREAD_CONTEXT * context,
	_In_ BYTE instructionLength,
	_In_ BYTE addressSize,
	_In_ BOOL hasRepPrefix,
	_In_ BOOL hasRepnePrefix,
	_Out_ MEMORY_ACCESS * memoryAccess,
	_Out_ BOOLEAN * isThereMemoryAccess
);


#endif // !_MEMORY_ACCESS_H_