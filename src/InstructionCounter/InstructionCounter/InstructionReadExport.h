#ifndef _INSTRUCTION_READ_EXPORT_H_
#define _INSTRUCTION_READ_EXPORT_H_


/**
 * Get the memory access done by an instruction
 *
 *
 * @param[in] pInstructionRead instruction pointer which does the memory access
 * @param[in] pContext context pointer of the thread before the instruction is executed.
 * @param[in] pMemoryAccess pointer on the memory access.
 * @param[in] pIsThereMemoryAccess return TRUE if there is a memory access, FALSE otherwise.
 * @return TRUE if the function succeed, FALSE otherwise.
 */
INSTRUCTIONCOUNTER_LIB BOOL GetMemoryAccessFromInstructionRead(
	_In_ const INSTRUCTION_READ * pInstructionRead,
	_In_ const THREAD_CONTEXT * pContext,
	_Out_ MEMORY_ACCESS * pMemoryAccess,
	_Out_ BOOLEAN * pIsThereMemoryAccess
);

/**
 * Return a nop instruction. A nop instruction is a instruction which does nothing.
 *
 *
 * @return a nop instruction.
 */
INSTRUCTIONCOUNTER_LIB INSTRUCTION_READ NopInstruction();

/**
 * Get information about the GS use by the instruction passed as parameter
 *
 * @param[in] pInstructionRead instruction pointer which use (or not) gs
 * @return TRUE if the instruction use GS, FALSE otherwise
 */
INSTRUCTIONCOUNTER_LIB BOOL UseGs(_In_ INSTRUCTION_READ * pInstructionRead);

/**
 * Get the value of the ModRM reg from the instruction passed as parameter
 *
 * @param[in] pInstructionRead instruction pointer which use ModRM.
 * @param[in] pContext context pointer of the thread before the instruction is executed.
 * @param[in] pValue return, on success only, the value of the ModRM.
 * @return TRUE if the function succeed, FALSE otherwise.
 */
INSTRUCTIONCOUNTER_LIB _Success_(return) BOOL GetModRMRegFromInstructionRead(_In_ INSTRUCTION_READ * pInstructionRead, _In_ const THREAD_CONTEXT * pContext, _Out_ QWORD * pValue);

/**
 * Used internally
 */
INSTRUCTIONCOUNTER_LIB DWORD movRdiModRM(_In_ INSTRUCTION_READ * pInstruction, _In_opt_ OPCODE * buffer);

#endif // !_INSTRUCTION_READ_EXPORT_H_
