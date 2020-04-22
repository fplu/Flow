#ifndef _TreeINSTRUCTIONCOMPTEUR_H_
#define _TreeINSTRUCTIONCOMPTEUR_H_

#define RESOURCE_OPCODE _T("\\resourceOpcode.txt")

#define PREFIXEIMM16ORIMM32MODE 0x66



/*! field of shared between INSTRUCTION and INSTRUCTION_READ */
typedef struct INSTRUCTION_SHARED_VALUE_s {
	BOOLEAN IsRipRelative;	//!< TRUE if the instruction result depends on RIP, FALSE otherwise.
	BOOLEAN IsRet;			//!< TRUE if the instruction is a ret, FALSE otherwise.
	BOOLEAN IsJcc;			//!< TRUE if the instruction is a jcc (Jump if Condition Is Met), FALSE otherwise.
	BOOLEAN IsLoop;			//!< TRUE if the instruction is a loop, FALSE otherwise.
	BOOLEAN IsCall;			//!< TRUE if the instruction is a call, FALSE otherwise.
	BOOLEAN IsJmp;			//!< TRUE if the instruction is a jmp, FALSE otherwise.


	MEMORY_ACCESS_INFORMATION MemoryAccess; //!< information about the possible memory access done by the instruction.

	void * UserData;		//!< Used internally by FLOW. Do not modify it if you are using FLOW. Set to NULL per default.

	char * Text;			//!< Raw text information about the instruction (mnemonic and raw operand). It is not the disassembled.
}INSTRUCTION_SHARED_VALUE;

/**
 * Used internally
 */
typedef struct INSTRUCTION_s {
	struct INSTRUCTION_s* Flink;
	struct INSTRUCTION_s* Blink;
	union {
		QWORD _andq;
		OPCODE _andb[8];
	};
	union {
		QWORD _xorq;
		OPCODE _xorb[8];
	};
	BYTE ModRMPos;
	BYTE Imm32_16Pos;
	BYTE Length;
	BOOLEAN IsVex;
	BOOLEAN _0FOpcode;

	BOOLEAN AlwaysHasOperandSizePrefix;
	BOOLEAN AlwaysHasAddressSizePrefix; // 4 or 8 (8 is default value)
	BOOLEAN AlwaysHasLockPrefix;
	BOOLEAN AlwaysHasRepPrefix;
	BOOLEAN AlwaysHasRepnePrefix;

	union {
		INSTRUCTION_SHARED_VALUE;
		INSTRUCTION_SHARED_VALUE CommonValue;
	};
}INSTRUCTION;

/*! Information about an instruction read using GetInstruction function*/
typedef struct INSTRUCTION_READ_s {
	OPCODE * Opcode;			//!< Address of the instruction (include prefix).
	BYTE Length;				//!< Length/Size of the instruction.
	BYTE EndOfPrefix;			//!< Opcode + EndOfPrefix = address of the first opcode inside the instruction which is not a prefix.

	BYTE ModRMPos;				//!< Opcode + ModRMPos = address of the ModRM opcode inside the instruction. If there is no ModRM, this field is equal to 255.
	REX Rex;					//!< Rex prefix. 0 if the instruction has no rex prefix.

	BOOLEAN HasOperandSizePrefix;//!< TRUE if the instruction has the "operand-size override prefix", FALSE otherwise.
	BYTE AddressSize;			 //!< 4 or 8 (8 is default value). 4 only if the instruction has the "address-size override prefix".
	BOOLEAN HasLockPrefix;		 //!< TRUE if the instruction has the "lock prefix", FALSE otherwise.
	BOOLEAN HasRepePrefix;		 //!< TRUE if the instruction has the "repe prefix", FALSE otherwise.
	BOOLEAN HasRepnePrefix;		 //!< TRUE if the instruction has the "repne prefix", FALSE otherwise.

	union { //! Other information about the instruction.
		INSTRUCTION_SHARED_VALUE;
		INSTRUCTION_SHARED_VALUE CommonValue;
	};
}INSTRUCTION_READ;


/**
 * Used internally
 */
typedef struct INSTRUCTION_TREE_s {
	union {
		struct INSTRUCTION_TREE_s * Son[256];
		INSTRUCTION * Instruction[256];
	};
	BYTE Layer;
}INSTRUCTION_TREE;

extern TCHAR resourceDllPath[500];


/**
 * Used internally
 */
BOOL CreateEntireInstructionTree();
/**
 * Used internally
 */
_Success_(return) BOOL FormatDB(_Out_ char ** file);
/**
 * Used internally
 */
BOOL CreateInstructionTree(_In_ char * pInstructionFile);
/**
 * Used internally
 */
BOOL CreateInstructionNode(
	_In_ BYTE _0[5], 
	_In_ BYTE _1[5], 
	_In_  BYTE length, 
	_In_ BYTE modRMPos, 
	_In_ BYTE imm32_16Pos, 
	_In_ char * text, 
	_In_ MEMORY_ACCESS_INFORMATION memoryAccessInformation,
	_Out_ INSTRUCTION * res);

/**
 * Used internally
 */
BOOL PutInstructionInTree(_In_ INSTRUCTION* INSTRUCTION, _Inout_ INSTRUCTION_TREE * head);



/**
 * Get instruction starting at opcode
 *
 *
 * @param[in] opcode start of the instruction (include prefixe).
 * @param[out] instructionRead information about the instruction located at opcode if the function success. Otherwise, it does not modify the value.
 * @return TRUE if opcode was a valid instruction.
 */
INSTRUCTIONCOUNTER_LIB _Success_(return) BOOL GetInstruction(_In_ const OPCODE * opcode, _Out_ INSTRUCTION_READ * instructionRead);

/**
 * Get the instruction string
 *
 *
 * @param[in] instructionRead the instruction to convert into string
 * @param[out] str the string, it should be 100 char length.
 * @return TRUE if the function succeed, FALSE otherwise.
 */
INSTRUCTIONCOUNTER_LIB BOOL GetInstructionStr(_In_ INSTRUCTION_READ* instructionRead, _Out_ char str[100]);


#endif