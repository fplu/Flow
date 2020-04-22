#include "MainINSTRUCTIONCounter.h"

//attention prefix
//extern Opcode INSTRUCTIONPrefix[];
OPCODE INSTRUCTIONPrefix_[] = { 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67, 0xF0, 0xF2, 0xF3, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F };

readPrefixeFunction readPrefixeFunctions[256];

#define funcRexPrefixe(id)\
BOOL IsRexPrefix ## id(INSTRUCTION_READ * instructionRead) {\
	instructionRead->Rex.EntireByte = id;\
	return TRUE;\
}


BOOL IsNotAPrefix(INSTRUCTION_READ * instructionRead) {
	return FALSE;
}

BOOL IsAPrefix(INSTRUCTION_READ * instructionRead) {
	return TRUE;
}

BOOL IsOperandSizePrefix(INSTRUCTION_READ * instructionRead) {
	instructionRead->HasOperandSizePrefix = TRUE;
	return TRUE;
}

BOOL IsAddressSizePrefix(INSTRUCTION_READ * instructionRead) {
	instructionRead->AddressSize = 4;
	return TRUE;
}

BOOL IsLockPrefix(INSTRUCTION_READ * instructionRead) {
	instructionRead->HasLockPrefix = TRUE;
	return TRUE;
}

BOOL IsRepePrefix(INSTRUCTION_READ * instructionRead) {
	instructionRead->HasRepePrefix = TRUE;
	return TRUE;
}

BOOL IsRepnePrefix(INSTRUCTION_READ * instructionRead) {
	instructionRead->HasRepnePrefix = TRUE;
	return TRUE;
}

funcRexPrefixe(0x40);
funcRexPrefixe(0x41);
funcRexPrefixe(0x42);
funcRexPrefixe(0x43);
funcRexPrefixe(0x44);
funcRexPrefixe(0x45);
funcRexPrefixe(0x46);
funcRexPrefixe(0x47);
funcRexPrefixe(0x48);
funcRexPrefixe(0x49);
funcRexPrefixe(0x4A);
funcRexPrefixe(0x4B);
funcRexPrefixe(0x4C);
funcRexPrefixe(0x4D);
funcRexPrefixe(0x4E);
funcRexPrefixe(0x4F);


BOOL InitReadPrefixe(void) {
	int i;

	for (i = 0; i < 256; i++) {
		readPrefixeFunctions[i] = IsNotAPrefix;
	}

	for (i = 0; i < sizeof(INSTRUCTIONPrefix_); i++) {
		readPrefixeFunctions[INSTRUCTIONPrefix_[i]] = IsAPrefix;
	}

	readPrefixeFunctions[OPERAND_SIZE_PREFIX] = IsOperandSizePrefix;
	readPrefixeFunctions[ADDRESS_SIZE_PREFIX] = IsAddressSizePrefix;
	readPrefixeFunctions[LOCK_PREFIX] = IsLockPrefix;
	readPrefixeFunctions[REPE_PREFIX] = IsRepePrefix;
	readPrefixeFunctions[REPNE_PREFIX] = IsRepnePrefix;

	readPrefixeFunctions[0x40] = IsRexPrefix0x40;
	readPrefixeFunctions[0x41] = IsRexPrefix0x41;
	readPrefixeFunctions[0x42] = IsRexPrefix0x42;
	readPrefixeFunctions[0x43] = IsRexPrefix0x43;
	readPrefixeFunctions[0x44] = IsRexPrefix0x44;
	readPrefixeFunctions[0x45] = IsRexPrefix0x45;
	readPrefixeFunctions[0x46] = IsRexPrefix0x46;
	readPrefixeFunctions[0x47] = IsRexPrefix0x47;
	readPrefixeFunctions[0x48] = IsRexPrefix0x48;
	readPrefixeFunctions[0x49] = IsRexPrefix0x49;
	readPrefixeFunctions[0x4A] = IsRexPrefix0x4A;
	readPrefixeFunctions[0x4B] = IsRexPrefix0x4B;
	readPrefixeFunctions[0x4C] = IsRexPrefix0x4C;
	readPrefixeFunctions[0x4D] = IsRexPrefix0x4D;
	readPrefixeFunctions[0x4E] = IsRexPrefix0x4E;
	readPrefixeFunctions[0x4F] = IsRexPrefix0x4F;

	return TRUE;
}