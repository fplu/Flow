#ifndef _INC_RIP_H_
#define _INC_RIP_H_




typedef struct CALL_STACK_ELEMENT_s {
	QWORD OldAddress;
	QWORD NewAddress;
	QWORD Callback;
}CALL_STACK_ELEMENT, * PCALL_STACK_ELEMENT;

typedef struct DOCKER_PAGE_ {
	void * base;
	QWORD size;
}DOCKER_PAGE;

BOOL IsPrbINSTRUCTION(INSTRUCTION_READ* instructionRead);
DWORD SingleStepHandlerMemoryCheck(PMEMORY_BASIC_INFORMATION memBasicInfo, BYTE* ptr, SIZE_T size);
void SingleStepHandlerRun(DOCKER* docker);

#define isSyscall(rip) \
	(((BYTE*)(rip))[0] == 0x0F && ((BYTE*)(rip))[1] == 0x05)


#define CALLBACK_WRITE_SYSCALL() \
	if (2 + (*bufferIndex) + JMP_REL32_SIZE >= BUFFER_OPCODE_SIZE) {\
			success = FALSE; \
			__leave; \
	}\
	buffer[(*bufferIndex)] = 0x0F; \
	buffer[(*bufferIndex) + 1] = 0x05; \
	(*bufferIndex) += 2;


#endif // !_INC_RIP_H_
