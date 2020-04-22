#include "MainINSTRUCTIONCounter.h"


QWORD getRegisterValue(BYTE id, REX rex, BYTE operandSize, BOOLEAN rexBit, const THREAD_CONTEXT * context) {
	QWORD res = 0;



	if (operandSize == 1) {
		const BYTE _8bitsRegister[] = {
			(BYTE)(context->Rax),  (BYTE)(context->Rcx),  (BYTE)(context->Rdx),  (BYTE)(context->Rbx),  (BYTE)(context->Rax >> 8),	(BYTE)(context->Rcx >> 8),	(BYTE)(context->Rdx >> 8),	(BYTE)(context->Rbx >> 8),
			(BYTE)(context->Rax),  (BYTE)(context->Rcx),  (BYTE)(context->Rdx),  (BYTE)(context->Rbx),  (BYTE)(context->Rsp),		(BYTE)(context->Rbp),		(BYTE)(context->Rsi),		(BYTE)(context->Rdi),
			(BYTE)(context->R8),   (BYTE)(context->R9),   (BYTE)(context->R10),  (BYTE)(context->R11),  (BYTE)(context->R12),		(BYTE)(context->R13),		(BYTE)(context->R14),		(BYTE)(context->R15)
		};

		if (rex.Signature != REX_SIGNATURE) {
			res = (QWORD)_8bitsRegister[id];
		}
		else if (rexBit) {
			res = (QWORD)_8bitsRegister[id + 16];
		}
		else {
			res = (QWORD)_8bitsRegister[id + 8];
		}
	}
	else if (operandSize == 2) {
		const WORD _16bitsRegister[] = {
			(WORD)context->Rax,  (WORD)context->Rcx,  (WORD)context->Rdx,  (WORD)context->Rbx,  (WORD)context->Rsp,  (WORD)context->Rbp,  (WORD)context->Rsi,  (WORD)context->Rdi,
			(WORD)context->R8,   (WORD)context->R9,   (WORD)context->R10,  (WORD)context->R11,  (WORD)context->R12,  (WORD)context->R13,  (WORD)context->R14,  (WORD)context->R15
		};
		res = (QWORD)_16bitsRegister[id + (rexBit << 3)];
	}
	else if (operandSize == 4) {
		const DWORD _32bitsRegister[] = {
			(DWORD)context->Rax,  (DWORD)context->Rcx, (DWORD)context->Rdx, (DWORD)context->Rbx, (DWORD)context->Rsp,  (DWORD)context->Rbp,  (DWORD)context->Rsi,  (DWORD)context->Rdi,
			(DWORD)context->R8,   (DWORD)context->R9,  (DWORD)context->R10, (DWORD)context->R11, (DWORD)context->R12,  (DWORD)context->R13,  (DWORD)context->R14,  (DWORD)context->R15
		};
		res = (QWORD)_32bitsRegister[id + (rexBit << 3)];
	}
	else if (operandSize == 8) {
		const QWORD _64bitsRegister[] = {
			context->Rax,  context->Rcx,  context->Rdx,  context->Rbx,  context->Rsp,  context->Rbp,  context->Rsi,  context->Rdi,
			context->R8,   context->R9,   context->R10,  context->R11,  context->R12,  context->R13,  context->R14,  context->R15
		};
		res = _64bitsRegister[id + (rexBit << 3)];
	}
	return res;
}

const char * getRegisterString(BYTE id, REX rex, BYTE operandSize, BOOLEAN rexBit) {
	const char * res = NULL;
	static const char * _64bitsRegister[] = {
		"Rax",  "Rcx",  "Rdx",  "Rbx",  "Rsp",  "Rbp",  "Rsi",  "Rdi",
		"R8",   "R9",   "R10",  "R11",  "R12",  "R13",  "R14",  "R15"
	};
	static const char * _32bitsRegister[] = {
		"Eax",  "Ecx",  "Edx",  "Ebx",  "Esp",  "Ebp",  "Esi",  "Edi",
		"R8D",   "R9D",   "R10D",  "R11",  "R12D",  "R13D",  "R14D",  "R15D"
	};
	static const char * _16bitsRegister[] = {
		"ax",  "cx",  "dx",  "bx",  "sp",  "bp",  "si",  "di",
		"R8W",   "R9W",   "R10W",  "R11W",  "R12W",  "R13W",  "R14W",  "R15W"
	};

	static const char * _8bitsRegister[] = {
		"al",  "cl",  "dl",  "bl", "ah",  "ch",  "dh",  "bh",
		"al",  "cl",  "dl",  "bl", "spl",  "bpl",  "sil",  "dil",
		"R8L",   "R9L",   "R10L",  "R11L",  "R12L",  "R13L",  "R14L",  "R15L"
	};

	if (operandSize == 1) {
		if (rex.Signature != REX_SIGNATURE) {
			res = _8bitsRegister[id];
		}
		else if (rexBit) {
			res = _8bitsRegister[id + 16];
		}
		else {
			res = _8bitsRegister[id + 8];
		}
	}
	else if (operandSize == 2) {
		res = _16bitsRegister[id + (rexBit << 3)];
	}
	else if (operandSize == 4) {
		res = _32bitsRegister[id + (rexBit << 3)];
	}
	else if (operandSize == 8) {
		res = _64bitsRegister[id + (rexBit << 3)];
	}

	return res;
}

/*
	ignore reg field, doesn't handle float
	Opcode concerne les bytes après le modrm (non compris)

	rex == 0 if no rex present
*/
QWORD GetModRMValue(
	_In_ MODRM modrm, 
	_In_ REX rex,
	_In_ const OPCODE * opcode,
	_In_ const THREAD_CONTEXT * context,
	_In_ BYTE instructionLength,
	_In_ BYTE operandSize,
	_In_ BYTE addressSize
) {
	void * addressToRead = NULL;
	QWORD valueRead = 0;
	BOOLEAN isThereMemoryAccess;
	

	if (modrm.Mod == 3) {
		return getRegisterValue(modrm.Rm, rex, operandSize, rex.B, context);
	}
	else {
		addressToRead = GetModRMAddress(modrm, rex, opcode, context, instructionLength, addressSize, &isThereMemoryAccess);
	}

	if (operandSize == 1) {
		valueRead = *(char*)addressToRead;
	}
	else if (operandSize == 2) {
		valueRead = *(short*)addressToRead;
	}
	if (operandSize == 4) {
		valueRead = *(int*)addressToRead;
	}
	if (operandSize == 8) {
		valueRead = *(long long*)addressToRead;
	}
	
	return valueRead;
}


/*
	ignore reg field, doesn't handle float
	Opcode concerne les bytes après le modrm (non compris)

	rex == 0 if no rex present
*/
void * GetModRMAddress(
	_In_ MODRM modrm, 
	_In_ REX rex,
	_In_ const OPCODE * opcode,
	_In_ const THREAD_CONTEXT * context,
	_In_ BYTE instructionLength,
	_In_ BYTE addressSize,
	_Out_ BOOLEAN * isThereMemoryAccess) {
	
	void* addressRes = NULL;

	*isThereMemoryAccess = TRUE;

	if (modrm.Mod == 3) {
		*isThereMemoryAccess = FALSE;
	}
	else if (modrm.Rm == 4) {
		addressRes = (void*)GetSIBValue(*(SIB *)(opcode), modrm, rex, opcode + 1, context);
	}
	else if (modrm.Rm == 5 && modrm.Mod == 0) {
		addressRes = (void*)(*(int*)(opcode)+context->Rip + instructionLength);
	}
	else {
		addressRes = (void*)getRegisterValue(modrm.Rm, rex, addressSize, rex.B, context);
	}


	if (modrm.Mod == 0) {
	}
	else if (modrm.Mod == 1) {
		if (modrm.Rm == 4) {//décalage pour le sib byte
			(BYTE*)addressRes += *(char*)(opcode + 1);
		}
		else {
			(BYTE*)addressRes += *(char*)(opcode);
		}
	}
	else if (modrm.Mod == 2) {
		if (modrm.Rm == 4) {//décalage pour le sib byte
			(BYTE*)addressRes += *(int*)(opcode + 1);
		}
		else {
			(BYTE*)addressRes += *(int*)(opcode);
		}
	}

	return addressRes;
}

/*
	Opcode concerne les bytes après le sib (non compris)
*/
QWORD GetSIBValue(SIB sib, MODRM modrm, REX rex, const OPCODE * Opcode, const THREAD_CONTEXT * context) {
	QWORD addressToRead = 0;
	QWORD scale = 0;
	QWORD valueRead = 0;


	scale = (QWORD)1 << sib.Scale;
	if (sib.Index == 4 && (rex.X == 0 || rex.Signature != REX_SIGNATURE)) {
		addressToRead = 0;
	}
	else {
		addressToRead = getRegisterValue(sib.Index, rex, 8, rex.X, context);
	}
	(QWORD)addressToRead *= scale;

	if (sib.Base == 5 && modrm.Mod == 0) {
		addressToRead += *(int*)(Opcode);
	}
	else {
		addressToRead += getRegisterValue(sib.Base, rex, 8, rex.B, context);
	}

	return (QWORD)addressToRead;
}

BYTE GetModRmSize(const OPCODE * Opcode) {
	MODRM modrm = *(MODRM*)Opcode;
	SIB sib;
	BYTE res;
	//Length added by the modrm byte
	static BYTE modRmTabSize[256] = { 0 };

	if (modRmTabSize[4] == 0) {
		int i;
		//Initialise l'impact du modRmByte
		for (i = 0; i <= 0x3F; i++) {
			if (i % 8 == 4) modRmTabSize[i]++;
			if (i % 8 == 5) modRmTabSize[i] += 4;
		}
		for (; i <= 0x7F; i++) {
			modRmTabSize[i]++;
			if (i % 8 == 4) modRmTabSize[i] ++;
		}
		for (; i <= 0xBF; i++) {
			modRmTabSize[i] += 4;
			if (i % 8 == 4) modRmTabSize[i] ++;
		}
	}

	res = modRmTabSize[Opcode[0]];

	if (modrm.Mod == 0 && modrm.Rm == 4) {
		sib = *(SIB*)(Opcode+1);
		if (sib.Base == 5) {
			res += 4;
		}
	}

	return res;
}

BOOLEAN GetRmString(MODRM modrm, REX rex, const OPCODE * Opcode, const void * rip, BYTE INSTRUCTIONSize, BYTE operandSize, BOOLEAN addressSize, char * str) {
	char value[40];
	const char * res = NULL;
	int size;
	
	if (modrm.Mod != 3) {
		if (operandSize == 1) {
			memCopy(str, "BYTE PTR [", 11);
			str += 10;
		}
		else if (operandSize == 2) {
			memCopy(str, "WORD PTR [", 11);
			str += 10;
		}
		else if (operandSize == 4) {
			memCopy(str, "DWORD PTR [", 12);
			str += 11;
		}
		else if (operandSize == 8) {
			memCopy(str, "QWORD PTR [", 12);
			str += 11;
		}

		if (modrm.Rm == 4 && modrm.Mod != 3) {
			getSIBString(*(SIB *)(Opcode), modrm, rex, Opcode + 1, addressSize, value);
			res = value;
		}
		else if (modrm.Rm == 5 && modrm.Mod == 0) {
			qwordToHexStr(*(int*)(Opcode)+ (QWORD)rip + INSTRUCTIONSize, value);
			res = value;
		}
		else {
			res = getRegisterString(modrm.Rm, rex, addressSize, rex.B);
		}

		size = (BYTE)getStrLength(res);
		memCopy(str, res, size + 1);
		str += size;

		if (modrm.Mod == 0) {
			value[0] = '\0';
			res = value;
		}
		else if (modrm.Mod == 1) {
			if (modrm.Rm == 4) {//décalage pour le sib byte
				value[0] = '+';
				qwordToHexStr(*(char*)(Opcode + 1), value + 1);
				res = value;
			}
			else {
				value[0] = '+';
				qwordToHexStr(*(char*)(Opcode), value + 1);
				res = value;
			}
		}
		else if (modrm.Mod == 2) {
			if (modrm.Rm == 4) {//décalage pour le sib byte
				value[0] = '+';
				qwordToHexStr(*(int*)(Opcode + 1), value + 1);
				res = value;
			}
			else {
				value[0] = '+';
				qwordToHexStr(*(int*)(Opcode), value + 1);
				res = value;
			}
		}

		size = (BYTE)getStrLength(res);
		memCopy(str, res, size + 1);
		str += size;

		str[0] = ']';
		str[1] = '\0';
	}
	else {//modrm.Mod == 3
		res = getRegisterString(modrm.Rm, rex, operandSize, rex.B);

		size = (BYTE)getStrLength(res);
		memCopy(str, res, size + 1);
		str += size;

		*str = '\0';
	}



	return TRUE;

}

BOOLEAN GetRegString(MODRM modrm, REX rex, BYTE operandSize, char * str) {
	const char * res = NULL;
	BYTE size;

	res = getRegisterString(modrm.Reg, rex, operandSize, rex.R);

	size = (BYTE)getStrLength(res);
	memCopy(str, res, size + 1);
	str += size;

	*str = '\0';

	return TRUE;

}

BOOLEAN getSIBString(SIB sib, MODRM modrm, REX rex, const OPCODE * Opcode, BOOLEAN addressSize, char * str) {

	QWORD * addressToRead = NULL;
	QWORD valueRead = 0;
	char value[20];
	const char * res;
	BYTE size;

	char * _scale[] = {
		"", "*2", "*4", "*8"
	};


	if (sib.Base == 5 && modrm.Mod == 0) {
		qwordToHexStr(*(int*)(Opcode + 1), value);
		res = value;
	}
	else {
		res = getRegisterString(sib.Base, rex, addressSize, rex.B);
	}
	

	size = (BYTE)getStrLength(res);
	memCopy(str, res, size + 1);
	str += size;


	if (sib.Index == 4) {
		value[0] = '\0';
		res = value;
	}
	else {
		*str = '+';
		str++;
		res = getRegisterString(sib.Index, rex, addressSize, rex.X);
	}
	
	
	size = (BYTE)getStrLength(res);
	memCopy(str, res, size + 1);
	str += size;




	res = _scale[sib.Scale];

	size = (BYTE)getStrLength(res);
	memCopy(str, res, size + 1);
	str += size;

	return TRUE;
}
	
