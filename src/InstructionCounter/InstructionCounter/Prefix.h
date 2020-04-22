#ifndef _PREFIXE_FUNCTION_H_
#define _PREFIXE_FUNCTION_H_
 
/**
 * Used internally.
 */
typedef BOOL(*readPrefixeFunction)(INSTRUCTION_READ * instructionRead);

#define IS_REX_PREFIX(Opcode) \
 Opcode >= 0x40 && Opcode <= 0x4F\

/**
 * Used internally.
 */
BOOL InitReadPrefixe(void);


#endif // !_PREFIXE_FUNCTION_H_
