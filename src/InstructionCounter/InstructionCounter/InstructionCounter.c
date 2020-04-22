#include "MainINSTRUCTIONCounter.h"



#define ASCIITOHEXA(hexa, c1, c2) \
	hexa = ('A' <= c1 && 'F' >= c1) ? (c1 - 'A' + 10) : (c1 - '0'); \
	hexa <<= 4; \
	hexa += ('A' <= c2 && 'F' >= c2) ? (c2 - 'A' + 10) : (c2 - '0')



//warning prefix
OPCODE g_InstructionPrefix[] = { 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67, 0xF0, 0xF2, 0xF3, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F };

extern readPrefixeFunction readPrefixeFunctions[256];


TCHAR resourceDllPath[500];

WORD cpt = 0;

BYTE * g_InstructionTreeBuffer = NULL;
DWORD g_InstructionTreeBufferLength;
INSTRUCTION_TREE g_Tree;



BOOL CreateEntireInstructionTree() {
	BOOL success = TRUE;
	OPCODE * fileOpcode = NULL;

	__try {
		if (!InitReadPrefixe()) {
			success = FALSE;
			__leave;
		}

		if (!FormatDB(&fileOpcode)) {
			success = FALSE;
			__leave;
		}

		if ((g_InstructionTreeBuffer = VirtualAlloc(NULL, 0x100000000, MEM_COMMIT, PAGE_READWRITE)) == NULL) {
			success = FALSE;
			__leave;
		}

		//we create the tree which allow to read instruction
		if (!CreateInstructionTree(fileOpcode)) {
			success = FALSE;
			__leave;
		}

		

	}
	__finally {
		if (success == FALSE) {
			if (g_InstructionTreeBuffer != NULL) {
				VirtualFree(g_InstructionTreeBuffer, 0, MEM_RELEASE);
				g_InstructionTreeBuffer = NULL;
			}
		}
	}

	return success;
}


//we format the file
_Success_(return) BOOL FormatDB(_Out_ char ** file) {
	BOOL success = TRUE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	char * fileRead = NULL;
	DWORD fileReadLength, numberOfBytesRead;

	__try {
		if (!file) {
			success = FALSE;
			__leave;
		}

		hFile = CreateFile(resourceDllPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			DebugBreak();
			success = FALSE;
			__leave;
		}
		
		fileReadLength = GetFileSize(hFile, NULL);
		fileReadLength++;

		*file = NULL;
		*file = (char*)calloc(fileReadLength, sizeof(char));
		if (!*file) {
			success = FALSE;
			__leave;
		}

		if (!ReadFile(hFile, *file, fileReadLength - 1, &numberOfBytesRead, NULL)) {
			success = FALSE;
			__leave;
		}
	}
	__finally {
		if (hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}
	}
	return success;
}

BOOL GetMemoryAccessInformation(_In_ char * text, _In_ BYTE modRmPos, _Out_ MEMORY_ACCESS_INFORMATION * memoryAccess) {
	BOOL success = TRUE;
	DWORD i;
	DWORD tmp[2];
	DWORD mCount;
	char tmpChar;
	DWORD virguleCount;
	DWORD mPos;

	__try {
		if (!text || !memoryAccess) {
			success = FALSE;
			__leave;
		}


		memoryAccess->Size = 0;
		memoryAccess->Operand = OPERAND_NO_ACCESS;


		tmp[0] = tmp[1] = virguleCount = mCount = mPos = 0;
		for (i = 0; text[i] != '\n' && text[i] != '\0' ; i++) {
			if (text[i] == 'N' && text[i + 1] == 'O' && text[i + 2] == 'P' && text[i + 3] == ' ') {
				__leave;
			}
			else if (text[i] == 'n' && text[i + 1] == 'o' && text[i + 2] == 'p' && text[i + 3] == ' ') {
				__leave;
			}
			else if (text[i] == 'L' && text[i + 1] == 'E' && text[i + 2] == 'A' && text[i + 3] == ' ') {
				__leave;
			}
			else if (text[i] == ',') {
				virguleCount++;
			}
			else if (text[i] == 'm' && i > 0 && text[i - 1] != 'm' && text[i + 1] >= '0' && text[i + 1] <= '9') {
				if (tmp[0] != 0) {
					tmp[0] = 0;
				}
				for (i++; text[i] >= '0'&& text[i] <= '9'; i++) {
					tmp[0] *= 10;
					tmp[0] += text[i] - '0';
				}
				if (text[i] == '&' || text[i] == ':' || text[i] == '/') {
					tmpChar = text[i];
					i++;
					for (; text[i] >= '0'&& text[i] <= '9'; i++) {
						tmp[1] *= 10;
						tmp[1] += text[i] - '0';
					}
					if (tmpChar == '/') {
						tmp[0] = tmp[1];
					}
					else {
						tmp[0] += tmp[1];
					}
				}
				mCount++;
				mPos = virguleCount;
			
				if (text[i] == ',') {
					virguleCount++;
				}
			}
			if (text[i] == '\n') {
				break;
			}
		}

		memoryAccess->Size = tmp[0];
		if (virguleCount == 1 && mCount == 2 && modRmPos == 255) {
			memoryAccess->Operand = OPERAND_RDI_AND_RSI_ACCESS;
			memoryAccess->Type = TYPE_READ_WRITE_ACCESS;
		}
		else if (mCount != 2 && modRmPos != 255) {
			memoryAccess->Operand = OPERAND_MODRM_ACCESS;		
			if (mPos == 0) {
				memoryAccess->Type = TYPE_WRITE_ACCESS;
			}
			else {
				memoryAccess->Type = TYPE_READ_ACCESS;
			}
		}
		else if (mCount == 1 && virguleCount == 0) {
			memoryAccess->Operand = OPERAND_RDI_ACCESS;
			memoryAccess->Type = TYPE_WRITE_ACCESS;
		}
		else if (mCount == 1 && virguleCount == 1) {
			memoryAccess->Operand = OPERAND_RSI_ACCESS;
			memoryAccess->Type = TYPE_READ_ACCESS;
		}
		else if (mCount == 0) {
			memoryAccess->Operand = OPERAND_NO_ACCESS;
			memoryAccess->Type = TYPE_NO_ACCESS;
		}
		else {
			DebugBreak();
		}
	}
	__finally {

	}

	return success;
}



BOOL CreateInstructionTree(_In_ char * pInstructionFile) {
	BOOL success = TRUE;

	BYTE modRMPos = 255;
	BYTE imm32_16Pos = 255;

	BYTE _0[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	BYTE _1[5] = { 0x00, 0x00, 0x00, 0x00, 0x00 };

	BYTE _01CPT = 0;

	char * text = NULL;
	BYTE firstAnd = FALSE;
	MEMORY_ACCESS_INFORMATION memoryAccessInformation;
	INSTRUCTION currentInstruction;

	int i;

	__try {
		
		ZeroMemory(&g_Tree, sizeof(INSTRUCTION_TREE));
	
		g_Tree.Layer = 0;


		for (i = 0; pInstructionFile[i] != '\0'; i++) {
			
			if (!firstAnd) {
				if (pInstructionFile[i] == '0') {
					if (_01CPT < 40) {
						_0[_01CPT / 8] &= (0xFF - (1 << (7 - (_01CPT & 7))));
					}
					_01CPT++;
				}
				else if (pInstructionFile[i] == '1') {
					if (_01CPT < 40) {
						_1[_01CPT / 8] |= (1 << (7 - (_01CPT & 7)));
					}
					_01CPT++;
				}
				else if (pInstructionFile[i] == 'X') {
					_01CPT++;
				}
				else if (pInstructionFile[i] == '/') {
					modRMPos = _01CPT / 8;
				}
				else if (pInstructionFile[i] == 'I') {
					imm32_16Pos = _01CPT / 8;
					_01CPT += 32;
				}
				else if (pInstructionFile[i] == '&') {
					text = pInstructionFile + i + 2;
					GetMemoryAccessInformation(text, modRMPos, &memoryAccessInformation);
					firstAnd = TRUE;
				}
			}
			else if (pInstructionFile[i] == '\n') {

				pInstructionFile[i] = '\0';
				

				if (!CreateInstructionNode(_0, _1, _01CPT / 8, modRMPos, imm32_16Pos, text, memoryAccessInformation, &currentInstruction)) {
					success = FALSE;
					__leave;
				}

				if (!PutInstructionInTree(&currentInstruction, &g_Tree)) {
					success = FALSE;
					__leave;
				}
				modRMPos = 255;
				imm32_16Pos = 255;
				_0[0] = _0[1] = _0[2] = _0[3] = _0[4] = 0xFF;
				_1[0] = _1[1] = _1[2] = _1[3] = _1[4] = 0x00;
				_01CPT = 0;
				firstAnd = FALSE;
			}
		}
	}
	__finally {

	}
	return success;
}


BOOL CreateInstructionNode(
	_In_ BYTE _0[5], 
	_In_ BYTE _1[5], 
	_In_  BYTE length, 
	_In_ BYTE modRMPos, 
	_In_ BYTE imm32_16Pos, 
	_In_ char * text, 
	_In_ MEMORY_ACCESS_INFORMATION memoryAccessInformation,
	_Out_ INSTRUCTION * res
) {
	BOOL success = TRUE;
	int i;
	INSTRUCTION instruction;
	BYTE and, xor, andXorIndex;

	__try {
		if (!res) {
			SetLastError(ERROR_INVALID_PARAMETER);
			success = FALSE;
			__leave;
		}

		instruction.UserData = NULL;

		instruction.Text = text;

		instruction.MemoryAccess = memoryAccessInformation;

		instruction.Imm32_16Pos = imm32_16Pos;
		instruction.ModRMPos = modRMPos;
		instruction.Length = length;

		instruction.AlwaysHasOperandSizePrefix = FALSE;
		instruction.AlwaysHasAddressSizePrefix = FALSE;
		instruction.AlwaysHasLockPrefix = FALSE;
		instruction.AlwaysHasRepPrefix = FALSE;
		instruction.AlwaysHasRepnePrefix = FALSE;

		instruction._0FOpcode = FALSE;

		instruction._andq = 0x0000000000000000;
		instruction._xorq = 0x0000000000000000;

		for (i = 0, andXorIndex = 0; i < 5; i++) {
			and = ~(_0[i] ^ _1[i]);
			xor = _1[i];
		
			if (_1[i] == 0x0F) {
				instruction._0FOpcode = TRUE;
			}
			
			if (andXorIndex) {
				(instruction._andb)[andXorIndex] = and;
				(instruction._xorb)[andXorIndex] = xor;
				andXorIndex++;
			}
			else if (OPERAND_SIZE_PREFIX == _1[i]) {
				instruction.AlwaysHasOperandSizePrefix = TRUE;
				instruction.Length--;
				if (instruction.ModRMPos != 255) {
					instruction.ModRMPos--;
				}
				else if (instruction.Imm32_16Pos != 255) {
					instruction.Imm32_16Pos--;
				}
			}
			else if (ADDRESS_SIZE_PREFIX == _1[i]) {
				instruction.AlwaysHasAddressSizePrefix = TRUE;
				instruction.Length--;
				if (instruction.ModRMPos != 255) {
					instruction.ModRMPos--;
				}
				else if (instruction.Imm32_16Pos != 255) {
					instruction.Imm32_16Pos--;
				}
			}
			else if (LOCK_PREFIX == _1[i]) {
				instruction.AlwaysHasLockPrefix = TRUE;
				instruction.Length--;
				if (instruction.ModRMPos != 255) {
					instruction.ModRMPos--;
				}
				else if (instruction.Imm32_16Pos != 255) {
					instruction.Imm32_16Pos--;
				}
			}
			else if (REPE_PREFIX == _1[i]) {
				instruction.AlwaysHasRepPrefix = TRUE;
				instruction.Length--;
				if (instruction.ModRMPos != 255) {
					instruction.ModRMPos--;
				}
				else if (instruction.Imm32_16Pos != 255) {
					instruction.Imm32_16Pos--;
				}
			}
			else if (REPNE_PREFIX == _1[i]) {
				instruction.AlwaysHasRepnePrefix = TRUE;
				instruction.Length--;
				if (instruction.ModRMPos != 255) {
					instruction.ModRMPos--;
				}
				else if (instruction.Imm32_16Pos != 255) {
					instruction.Imm32_16Pos--;
				}
			}
			else {
				(instruction._andb)[andXorIndex] = and;
				(instruction._xorb)[andXorIndex] = xor;
				andXorIndex++;
			}

		}

		instruction.IsRipRelative = FALSE;
		instruction.IsVex = FALSE;

		//Stuff to detect rip relative INSTRUCTION which are not due to a modRM byte
		if (!((instruction._xorb)[0] ^ JMPREL32)) {
			instruction.IsRipRelative = TRUE;
		}
		else if (!((instruction._xorb)[0] ^ JMPREL8)) {
			instruction.IsRipRelative = TRUE;
		}
		else if (!((instruction._xorb)[0] ^ CALLREL32)) {
			instruction.IsRipRelative = TRUE;
		}

		if (!((instruction._xorb)[0] ^ 0xC4) || !((instruction._xorb)[0] ^ 0xC5)) {
			instruction.IsVex = TRUE;
		}


		//the jcc with two opcodes
		if (IsJccOrLoop(instruction._xorb)) {
			instruction.IsRipRelative = TRUE;
		}
		instruction.IsJcc = IsJcc(instruction._xorb);
		instruction.IsLoop = IsLoop(instruction._xorb);
		instruction.IsCall = IsCall(instruction._xorb);
		instruction.IsRet = IsRet(instruction._xorb);
		instruction.IsJmp = IsJmp(instruction._xorb);

		//syscall
		if (instruction._xorb[0] == 0x0F && instruction._xorb[1] == 0x05) {
			instruction.IsCall = TRUE;
		}

		(*res) = instruction;
		(res)->Flink = (res);
		(res)->Blink = (res);
	}
	__finally {

	}
	return success;
}


BOOL PutInstructionInTree(_In_ INSTRUCTION* instruction, _Inout_ INSTRUCTION_TREE * head) {
	BOOL success = TRUE;

	INSTRUCTION_TREE * nodeTree[5] = { NULL };
	INSTRUCTION_TREE * currentNodeTree;
	PLIST_BASIC listeCurrentNode = NULL;
	INSTRUCTION * tmpInstruction;
	BYTE i = 0;

	__try {
		if (!head || !instruction) {
			SetLastError(ERROR_INVALID_PARAMETER);
			success = FALSE;
			__leave;
		}

		//Tant qu'on a pas parcouru tout les noeud
		currentNodeTree = head;
		do {

			i = 0;//On parcours les 256 chemins potentiellement compatible pour chaque noeud
			do {
				if (!((i ^ instruction->_xorb[currentNodeTree->Layer]) & instruction->_andb[currentNodeTree->Layer])) {
					if (currentNodeTree->Layer < 5) {
						if (!currentNodeTree->Son[i]) {
							//Allocation si pas déjà fait
							if (nodeTree[currentNodeTree->Layer] == NULL) {
								//nodeTree[currentNodeTree->Layer] = calloc(sizeof(INSTRUCTION_TREE), 1);
								nodeTree[currentNodeTree->Layer] = (INSTRUCTION_TREE*)(g_InstructionTreeBuffer + g_InstructionTreeBufferLength);
								g_InstructionTreeBufferLength += sizeof(INSTRUCTION_TREE);
								ZeroMemory(nodeTree[currentNodeTree->Layer], sizeof(INSTRUCTION_TREE));
								nodeTree[currentNodeTree->Layer]->Layer = currentNodeTree->Layer + 1;
							}

							currentNodeTree->Son[i] = nodeTree[currentNodeTree->Layer];
							//nodeTreeUsed[currentNodeTree->Layer] = TRUE;
						}
						if (!IsDataInListBasic(currentNodeTree->Son[i], listeCurrentNode)) {
							if (!AddNodeListBasic(currentNodeTree->Son[i], sizeof(LIST_BASIC), &listeCurrentNode)) {
								success = FALSE;
								__leave;
							}
						}
					}
					else {
						//tmpInstruction = (INSTRUCTION *)malloc(sizeof(INSTRUCTION));
						tmpInstruction = (INSTRUCTION *)(g_InstructionTreeBuffer + g_InstructionTreeBufferLength);
						g_InstructionTreeBufferLength += sizeof(INSTRUCTION);
							
						*tmpInstruction = *instruction;
						tmpInstruction->Flink = tmpInstruction;
						tmpInstruction->Blink = tmpInstruction;
						if (!AddListBasic((PLIST_BASIC)tmpInstruction, (PLIST_BASIC *)&currentNodeTree->Instruction[i])) {
							success = FALSE;
							__leave;
						}
					}
				}
				i++;
			} while (i != 0);

			if (!listeCurrentNode) {
				break;
			}

			if (!PopNodeListBasic(&listeCurrentNode, NULL, &currentNodeTree)) {
				success = FALSE;
				__leave;
			}
		} while (1);
	}
	__finally {

	}
	return success;
}


/*
	To avoid any kind of buffer overflow Opcode must contain at least 16 Opcode (maximum size of an instruction)
*/
_Success_(return) BOOL GetInstruction(_In_ const OPCODE * opcode, _Out_ INSTRUCTION_READ * instructionRead) {
	BOOL success = TRUE;
	INSTRUCTION_TREE * currentNode;
	INSTRUCTION * currentInstruction;
	INSTRUCTION_READ res;
	int i, j;
	OPCODE* tmp = 0;
	QWORD opcodeQWORD;


	__try {
		if (!opcode || !instructionRead) {
			success = FALSE;
			__leave;
		}

		res.HasOperandSizePrefix = FALSE;
		res.AddressSize = 8;
		res.HasLockPrefix = FALSE;
		res.HasRepePrefix = FALSE;
		res.HasRepnePrefix = FALSE;
		res.Rex.EntireByte = 0;
		res.ModRMPos = 255;

		for (j = 0; j < MAX_INSTRUCTION_SIZE; j++) {

			//We search for an INSTRUCTION
			currentNode = &g_Tree;
			opcodeQWORD = 0;
			for (i = 0; i < 6; i++) {
				opcodeQWORD += opcode[j + i] * ((QWORD)0x1 << ((i * 2) << 2));
				if (!currentNode->Son[opcode[j + i]]) {
					currentNode = NULL;
					break;
				}
				currentNode = currentNode->Son[opcode[j + i]];
			}


			//if an INSTRUCTION was possibly found
			if (currentNode != NULL) {
				currentInstruction = (INSTRUCTION *)currentNode;
				do {
					//check INSTRUCTION validity
					if (!((opcodeQWORD ^ currentInstruction->_xorq) & currentInstruction->_andq)
						&& (!currentInstruction->AlwaysHasOperandSizePrefix || res.HasOperandSizePrefix)
						&& (!currentInstruction->AlwaysHasAddressSizePrefix || res.AddressSize == 4)
						&& (!currentInstruction->AlwaysHasLockPrefix || res.HasLockPrefix)
						&& (!currentInstruction->AlwaysHasRepPrefix || res.HasRepePrefix)
						&& (!currentInstruction->AlwaysHasRepnePrefix || res.HasRepnePrefix)
					) {

						res.CommonValue = currentInstruction->CommonValue;
				
						res.Length = currentInstruction->Length + j;

						//operand size prefix exception
						if (res.HasOperandSizePrefix && currentInstruction->Imm32_16Pos != 255) {
							res.Length -= 2;
							res.MemoryAccess.Size = 16;
						}

						res.Opcode = (OPCODE *)opcode;
						res.EndOfPrefix = j;


						//modRm byte exception
						if (currentInstruction->ModRMPos != 255) {
							if((*(MODRM*)&opcode[j + currentInstruction->ModRMPos]).Mod == 3) {
								res.MemoryAccess.Type = TYPE_NO_ACCESS;
								res.MemoryAccess.Operand = OPERAND_NO_ACCESS;
							}
							if (IS_MODRM_RIP_RELATIVE(opcode[j + currentInstruction->ModRMPos])) {
								res.IsRipRelative = TRUE;
							}
							res.ModRMPos = currentInstruction->ModRMPos + j;
							res.Length += GetModRmSize((OPCODE *)((ULONG_PTR)opcode + (ULONG_PTR)j + (ULONG_PTR)currentInstruction->ModRMPos));
						}


						if (IS_REX_PREFIX(opcode[j])) {
							res.Rex = *(REX*)(opcode + j);
							res.EndOfPrefix = j + 1;
						}
						else if (j > 0 && IS_REX_PREFIX(opcode[j - 1])) {
							res.Rex = *(REX*)(opcode + j - 1);
						}
						else {
							res.Rex.EntireByte = 0;
						}

						if (res.Rex.Signature == REX_SIGNATURE && res.Rex.W) {
							res.HasOperandSizePrefix = FALSE;
						}


						__leave;
					}
					currentInstruction = currentInstruction->Flink;
				} while ((ULONG_PTR)currentInstruction != (ULONG_PTR)currentNode);
			}

			//If no INSTRUCTIONs are found, it is possibly due to a prefix
			if (!readPrefixeFunctions[opcode[j]](&res)) {
				success = FALSE;
				__leave;
			}


		}
		success = FALSE;
	}
	__finally {
		if (success) {
			*instructionRead = res;
		}

	}
	return success;
}



BOOL GetInstructionStr(_In_ INSTRUCTION_READ * instructionRead, _Out_ char str[100]) {
	BOOL success = FALSE;
	HANDLE  hProcess;
	BYTE operandSize;
	char * text;
	int i;

	__try {
		if (instructionRead == NULL || str == NULL) {
			success = FALSE;
			__leave;
		}

		hProcess = GetCurrentProcess();

		text = instructionRead->Text;

		while (*text != ' ') {
			*str = *text;
			text++;
			str++;
		}

		*str = ' ';
		str++;
		*str = '\0';
		do {

			while (*text == ' ' || *text == ',') {
				text++;
			}

			if (*text == '\0' || *text == '\r') {
				break;
			}

			if (
				text[0] == 'r' &&
				text[1] == ' ' &&
				text[2] == '/' &&
				text[3] == 'm') {


				if (text[4] == '8') {
					operandSize = 1;
					text += 5;
				}
				else if (text[4] == '1' && text[5] == '6') {
					operandSize = 2;
					text += 6;
				}
				else if (text[4] == '3' && text[5] == '2') {
					operandSize = 4;
					text += 6;
				}
				else if (text[4] == '6' && text[5] == '4') {
					operandSize = 8;
					text += 6;
				}
				else {
					operandSize = 0;
					text += 6;
				}

				if (operandSize != 0) {
					if (instructionRead->ModRMPos != 255) {
						GetRmString(
							*(MODRM*)(instructionRead->Opcode + instructionRead->ModRMPos),
							instructionRead->Rex,
							instructionRead->Opcode + instructionRead->ModRMPos + 1,
							instructionRead->Opcode,
							instructionRead->Length,
							operandSize,
							instructionRead->AddressSize,
							str);
					}

				}
			}
			else if (text[0] == 'r') {

				if (text[1] == '8') {
					operandSize = 1;
					text += 2;
				}
				else if (text[1] == '1' && text[2] == '6') {
					operandSize = 2;
					text += 3;
				}
				else if (text[1] == '3' && text[2] == '2') {
					operandSize = 4;
					text += 3;
				}
				else if (text[1] == '6' && text[2] == '4') {
					operandSize = 8;
					text += 3;
				}
				else {
					operandSize = 0;
					text += 3;
				}

				if (operandSize != 0) {
					if (instructionRead->ModRMPos != 255) {
						GetRegString(
							*(MODRM*)(instructionRead->Opcode + instructionRead->ModRMPos),
							instructionRead->Rex,
							operandSize,
							str);
					}
					else {
						GetRegString(
							*(MODRM*)(instructionRead->Opcode + instructionRead->EndOfPrefix),
							instructionRead->Rex,
							operandSize,
							str);
					}
				}
			}
			else if (
				text[0] == 'i' &&
				text[1] == 'm' &&
				text[2] == 'm') {

				if (text[3] == '8') {
					qwordToHexStr(*(unsigned char*)(instructionRead->Opcode + instructionRead->Length - 1), str);
					operandSize = 1;
					text += 4;
				}
				else if (text[3] == '1' && text[4] == '6') {
					qwordToHexStr(*(unsigned short*)(instructionRead->Opcode + instructionRead->Length - 2), str);
					operandSize = 2;
					text += 5;
				}
				else if (text[3] == '3' && text[4] == '2') {
					qwordToHexStr(*(unsigned int*)(instructionRead->Opcode + instructionRead->Length - 4), str);
					operandSize = 4;
					text += 5;
				}
				else if (text[3] == '6' && text[4] == '4') {
					qwordToHexStr(*(unsigned long long*)(instructionRead->Opcode + instructionRead->Length - 8), str);
					operandSize = 8;
					text += 5;
				}
			}
			else {
				for (i = 0; *text != ' ' && *text != ',' && *text != '\0' && *text != '\r'; i++) {
					str[i] = *text;
					text++;
				}
				str[i] = '\0';
			}

			if (*text == '\0' || *text == '\r') {
				break;
			}

			while (*text == ' ' || *text == ',') {
				text++;
			}

			if (*text == '\0' || *text == '\r') {
				break;
			}

			while (*str != '\0') {
				str++;
			}

			*str = ',';
			str++;
			*str = ' ';
			str++;
			*str = '\0';


		} while (1);


	}
	__finally {

	}

	return success;
}















