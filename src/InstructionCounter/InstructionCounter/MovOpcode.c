#include "MainINSTRUCTIONCounter.h"

DWORD g_TlsIdRipRelative = 0;

/*
	_In_ analyzedFunction->INSTRUCTIONsMaxLength size of already allocated analyzedFunction->INSTRUCTIONs
	_Out_ analyzedFunction->INSTRUCTIONsLength used size of already allocated analyzedFunction->INSTRUCTIONs

	_In_ analyzedFunction->OriginalAddress address inside the function to analyse
	_Out_ analyzedFunction->OriginalAddress address of a potential start of the function (no jump after this point which go before it)

	_In_ analyzedFunction->OriginalAddressLength max authorized size for the function

	_In_ useStack use the stack instead of the heap for the allocation inside the function
*/
/**/
BOOL ExtractEntireFunction(_Inout_ ANALYSED_FUNCTION * analyzedFunction, _In_ BOOLEAN useStack, _In_ BOOLEAN goAbove) {
	BOOL success = TRUE;
	DWORD * instructionMask = NULL;
	OPCODE * startAddress, *currentAddress, *endAddress;
	OPCODE * futureStartAddress, *oldStartAddress, *oldCurrentAddress;
	BOOLEAN endFind;
	OPCODE * jmpDest;
	DWORD i, instructionLength;
	ANALYSED_INSTRUCTION * instruction;
	ANALYSED_INSTRUCTION instructionStack;
	BOOLEAN basicMode ;

	if (useStack) {
		if ((instructionMask = _alloca(((size_t)analyzedFunction->OriginalAddressLength + (size_t)1) * sizeof(DWORD))) == NULL) {
			return FALSE;
		}
	}
	else {
		if((instructionMask = malloc(((size_t)analyzedFunction->OriginalAddressLength + (size_t)1) * sizeof(DWORD))) == NULL) {
			return FALSE;
		}
	}

	__try {

		if (analyzedFunction == NULL || analyzedFunction->OriginalAddress == NULL) {
			success = FALSE;
			__leave;
		}



		//STEP 1, find function bound
		instructionLength = 0;
		endFind = FALSE;
		basicMode = TRUE;

		futureStartAddress = currentAddress = endAddress = startAddress = analyzedFunction->OriginalAddress;

		do {
			for (i = 0; !endFind; i++) {
				instructionLength++;

				if (instructionLength > analyzedFunction->InstructionsMaxLength) {
					instructionLength--;
					break;
				}

				if (!GetInstruction(currentAddress, &(instructionStack.Instruction))) {
					analyzedFunction->Error = Data;
					success = FALSE;
					__leave;
				}

				if (!basicMode) {
					if (currentAddress + instructionStack.Length == oldStartAddress) {
						endFind = TRUE;
					}
					else if (currentAddress + instructionStack.Length >= oldStartAddress) {
						analyzedFunction->Error = AbnormalJmp;
						success = FALSE;
						__leave;
					}
				}

				if (instructionStack.IsJmp || instructionStack.IsJcc || instructionStack.IsLoop) {
					if (instructionStack.IsJmp) {
						jmpDest = (OPCODE*)GetJmpDest(instructionStack.Opcode + instructionStack.EndOfPrefix, instructionStack.Rex, currentAddress, instructionStack.Length, NULL, instructionStack.AddressSize);
					}
					else if (instructionStack.IsJcc || instructionStack.IsLoop) {
						jmpDest = (OPCODE*)GetJccOrLoopDest(instructionStack.Opcode + instructionStack.EndOfPrefix, currentAddress, instructionStack.Length);
					}

					if (jmpDest != NULL) {
						//If obliged jump at the end who go back in the function
						if (instructionStack.IsJmp && currentAddress == endAddress && jmpDest < currentAddress) {
							endFind = TRUE;
						}

						//If jump who go before the current start of the function
						if (jmpDest < futureStartAddress && endAddress < jmpDest + analyzedFunction->OriginalAddressLength) {
								futureStartAddress = jmpDest;
						}

						//If jump who go after the current end of the function
						if (jmpDest > endAddress && jmpDest < futureStartAddress + analyzedFunction->OriginalAddressLength) {
								endAddress = jmpDest;
						}
					}
					else if (currentAddress == endAddress) {
						endFind = TRUE;
					}
				}
				else if (instructionStack.IsRet) {
					if (currentAddress == endAddress) {
						endFind = TRUE;
					}
				}

				if (currentAddress + instructionStack.Length > endAddress) {
					//endAddress = currentAddress;
					if (currentAddress + instructionStack.Length < futureStartAddress + analyzedFunction->OriginalAddressLength) {
						currentAddress += instructionStack.Length;
						endAddress = currentAddress;
					}
					else if(currentAddress == endAddress) {
						instructionLength--;
						endFind = TRUE;
					}
					else {
						analyzedFunction->Error = AbnormalJmp;
						success = FALSE;
						__leave;
					}
				}
				else {
					currentAddress += instructionStack.Length;
				}

				if (endAddress >= futureStartAddress + analyzedFunction->OriginalAddressLength) {
					//DebugBreak();
					analyzedFunction->Error = OriginalFunctionTooBig;
					success = FALSE;
					__leave;
				}
			}

			//terminaison condition and switch mode
			if (instructionLength + 1 > analyzedFunction->InstructionsMaxLength) {
				break;
			}
			if (goAbove && futureStartAddress != startAddress) {
				oldStartAddress = startAddress;
				oldCurrentAddress = currentAddress; //currentAddress is egal to endAddress

				currentAddress = startAddress = futureStartAddress;

				endFind = FALSE;
				basicMode = FALSE;
			}
			else if (!basicMode && endAddress != oldCurrentAddress) {
				currentAddress = oldCurrentAddress; 

				endFind = FALSE;
				basicMode = TRUE;
			}
		} while (!endFind);

		analyzedFunction->InstructionsLength = instructionLength;

		//STEP 2 verify jump valilidiy

		//STEP 2.1 clear function mask
		for (i = 0; i < analyzedFunction->OriginalAddressLength + 1; i++) {
			instructionMask[i] = INVALID_INSTRUCTION_ID;
		}

		//STEP 2.2 fill function mask
		currentAddress = startAddress;
		for (i = 0; i < instructionLength; i++) {
			instruction = analyzedFunction->Instructions + i;
			if (!GetInstruction(currentAddress, &(instruction->Instruction))) {
				analyzedFunction->Error = Data;
				success = FALSE;
				__leave;
			}
			instruction->LinkedInstruction = NO_LINKED_INSTRUCTION;

			

			instructionMask[currentAddress - startAddress] = i;
			currentAddress += instruction->Length;
		}

		//step 2.3 check function jump validity and find their dest
		currentAddress = startAddress;
		for (i = 0; i < instructionLength; i++) {
			instruction = analyzedFunction->Instructions + i;

			if (instruction->IsJmp || instruction->IsJcc || instruction->IsLoop) {
				if (instruction->IsJmp) {
					jmpDest = (OPCODE*)GetJmpDest(instruction->Opcode + instruction->EndOfPrefix, instruction->Rex, currentAddress, instruction->Length, NULL, instruction->AddressSize);
				}
				else if (instruction->IsJcc || instruction->IsLoop) {
					jmpDest = (OPCODE*)GetJccOrLoopDest(instruction->Opcode + instruction->EndOfPrefix, currentAddress, instruction->Length);
				}

				if (jmpDest != NULL && jmpDest >= startAddress && jmpDest < endAddress) {
					if (instructionMask[jmpDest - startAddress] == INVALID_INSTRUCTION_ID) {
						analyzedFunction->Error = AbnormalJmp;
						success = FALSE;
						__leave;
					}
					else {
						instruction->LinkedInstruction = instructionMask[jmpDest - startAddress];
					}
				}
			}

			currentAddress += instruction->Length;
		}


	}
	__finally {
		if (!useStack && instructionMask != NULL) {
			free(instructionMask);
		}
	}

	return success;
}

BOOL GetNextRipValue(_In_ THREAD_CONTEXT * context, _In_ INSTRUCTION_READ * instructionRead, _In_ void * Rip, _Out_ void **  nextRip) {
	BOOL success = TRUE;
	QWORD res = (QWORD)NULL;
	OPCODE * instruction;

	__try {
		if (nextRip == NULL) {
			success = FALSE;
			__leave;
		}


		instruction = instructionRead->Opcode + instructionRead->EndOfPrefix;

		//we set nextRip with its default value then we handle the different particular case
		res = (QWORD)Rip + instructionRead->Length;
		if (instructionRead->IsJcc || instructionRead->IsLoop) {
			if (context == NULL) {
				success = FALSE;
				__leave;
			}
			if (IsJccOrLoopActive(instruction, (DWORD)context->EFlags, context->Rcx, instructionRead->AddressSize == 4)) {
				res = GetJccOrLoopDest(instruction, (const void*)context->Rip, instructionRead->Length);
			}
		}
		else if (instructionRead->IsJmp) {
			res = GetJmpDest(instruction, instructionRead->Rex, Rip, instructionRead->Length, context, instructionRead->AddressSize);
		}
		else if (instructionRead->IsCall) {
			res = GetCallDest(instruction, instructionRead->Rex, Rip, instructionRead->Length, context, instructionRead->AddressSize);
		}
		else if (instructionRead->IsRet) {
			if (context == NULL) {
				success = FALSE;
				__leave;
			}
			res = GetRetDest(instruction, (const void *)context->Rip, instructionRead->Length, (void*) context->Rsp, instructionRead->HasOperandSizePrefix);
		}

	}
	__finally {
		if ((void*)res == NULL) {
			success = FALSE;
		}
		if (success) {
			*nextRip = (void*)res;
		}
	}

	return success;
}


/*
	pBuf is no longer Rip relative but could need page execute read write access
	_In_ length what lengtt we need
	_Out_ length what length we get
*/
BOOL ModifyRipRelativeInstruction(_In_ OPCODE * start, _Inout_ DWORD * length, _Out_ OPCODE ** ppBuf, _Out_ DWORD * pBufLength) {
	BOOL success = TRUE;
	ANALYSED_FUNCTION analyzedBlock;
	ANALYSED_INSTRUCTION * tabInstructions = NULL;
	ANALYSED_INSTRUCTION * instruction;
	DWORD newFunctionLength;
	DWORD instructionLength;
	DWORD i;
	OPCODE * jmpDest;

	__try {
		if (start == NULL || length  == NULL || *length == 0 || ppBuf == NULL || pBufLength == NULL) {
			success = FALSE;
			__leave;
		}
		tabInstructions = malloc(sizeof(ANALYSED_INSTRUCTION) * *length);

		analyzedBlock.Instructions = tabInstructions;
		analyzedBlock.InstructionsMaxLength = *length;
		analyzedBlock.OriginalAddress = start;
		analyzedBlock.OriginalAddressLength = *length + 16;

		//get block and resolve jump
		if (!ExtractEntireFunction(&analyzedBlock, FALSE, FALSE)) {
			success = FALSE;
			__leave;
		}
		instructionLength = analyzedBlock.InstructionsLength;

		//find new INSTRUCTION size and relative new address
		*length = 0;
		newFunctionLength = 10;//save zone
		for (i = 0; i < instructionLength; i++) {
			instruction = analyzedBlock.Instructions + i;
			instruction->NewAddress = (OPCODE*)(QWORD)newFunctionLength;
			*length += instruction->Length;


			if (instruction->IsJmp && instruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
				jmpDest = (OPCODE*)GetJmpDest(instruction->Opcode + instruction->EndOfPrefix, instruction->Rex, instruction->Opcode, instruction->Length, NULL, instruction->AddressSize);

				if (jmpDest != NULL) {
					newFunctionLength += 14;
				}
				else {
					newFunctionLength += instruction->Length;
				}
			}
			else if ((instruction->IsJcc || instruction->IsLoop) && instruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
				if (instruction->Opcode[instruction->EndOfPrefix] == 0x0F) {//jmp rel 32
					newFunctionLength += 5;
				}
				else {
					newFunctionLength += 1;
				}
				newFunctionLength += instruction->EndOfPrefix + 17;
			}
			else if (instruction->IsJmp) {
				newFunctionLength += 5;
			}
			else if (instruction->IsJcc || instruction->IsLoop) {
				if (instruction->Opcode[instruction->EndOfPrefix] == 0x0F) {//jmp rel 32
					newFunctionLength += 6;
				}
				else {
					newFunctionLength += instruction->EndOfPrefix + 9;
				}
			}
			else if (instruction->IsRipRelative) {
				newFunctionLength += 28 + instruction->Length - 4;
			}
			else {
				newFunctionLength += instruction->Length;
			}
		}


		if (newFunctionLength < 2) {
			newFunctionLength = 2;
		}
		//create new INSTRUCTION
		if ((*ppBuf = malloc(sizeof(OPCODE) * newFunctionLength)) == NULL) {
			success = FALSE;
			__leave;
		}
		*pBufLength = newFunctionLength;
		(*ppBuf)[0] = JMP_REL8;
		(*ppBuf)[1] = 8;

		newFunctionLength = 10;
		for (i = 0; i < instructionLength; i++) {
			instruction = analyzedBlock.Instructions + i;

			//		(*ppBuf)

				if (instruction->IsJmp && instruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
					jmpDest = (OPCODE*)GetJmpDest(instruction->Opcode + instruction->EndOfPrefix, instruction->Rex, instruction->Opcode, instruction->Length, NULL, instruction->AddressSize);


					if (jmpDest != NULL) {
						jmpUsingRet((*ppBuf + newFunctionLength), (QWORD)jmpDest);
						newFunctionLength += 14;
					}
					else {
						memCopy((*ppBuf + newFunctionLength), instruction->Opcode, instruction->Length);
						newFunctionLength += instruction->Length;
					}
				}
				else if ((instruction->IsJcc || instruction->IsLoop) && instruction->LinkedInstruction == NO_LINKED_INSTRUCTION) {
					jmpDest = (OPCODE*)GetJccOrLoopDest(instruction->Opcode + instruction->EndOfPrefix, instruction->Opcode, instruction->Length);

					memCopy((*ppBuf + newFunctionLength), instruction->Opcode, instruction->EndOfPrefix + 1);
					newFunctionLength += instruction->EndOfPrefix + 1;

					if (instruction->Opcode[instruction->EndOfPrefix] == 0x0F) {//jmp rel 32
						(*ppBuf + newFunctionLength)[0] = instruction->Opcode[instruction->EndOfPrefix + 2]; newFunctionLength++;
						((DWORD*)(*ppBuf + newFunctionLength))[0] = 2; newFunctionLength+=4;
					}
					else {
						(*ppBuf + newFunctionLength)[0] = 2; newFunctionLength++;
					}

					(*ppBuf + newFunctionLength)[0] = JMP_REL8; newFunctionLength++;
					(*ppBuf + newFunctionLength)[0] = (BYTE)14; newFunctionLength++;

					jmpUsingRet((*ppBuf + newFunctionLength), (QWORD)jmpDest);
					newFunctionLength += 14;
				}
				else if (instruction->IsJmp) {
					jmpDest = analyzedBlock.Instructions[instruction->LinkedInstruction].NewAddress + (QWORD)*ppBuf;
					jmpRel32((*ppBuf + newFunctionLength), jmpDest);
					newFunctionLength += 5;
				}
				else if (instruction->IsJcc || instruction->IsLoop) {
					jmpDest = analyzedBlock.Instructions[instruction->LinkedInstruction].NewAddress + (QWORD)*ppBuf;

					if (instruction->Opcode[instruction->EndOfPrefix] == 0x0F) {//jmp rel 32
						jccRel32((*ppBuf + newFunctionLength), jmpDest, instruction->Opcode[instruction->EndOfPrefix + 1]);
						newFunctionLength += 6;
					}
					else {
						memCopy((*ppBuf + newFunctionLength), instruction->Opcode, instruction->EndOfPrefix + 1);
						newFunctionLength += instruction->EndOfPrefix + 1;

						(*ppBuf + newFunctionLength)[0] = 2; newFunctionLength++;
						(*ppBuf + newFunctionLength)[0] = JMP_REL8; newFunctionLength++;
						(*ppBuf + newFunctionLength)[0] = (BYTE)5; newFunctionLength++;

						jmpRel32((*ppBuf + newFunctionLength), jmpDest);
						newFunctionLength += 5;
					}
				}
				else if (instruction->IsRipRelative) {
					RewriteRipRelativeInstruction(&(instruction->Instruction), (*ppBuf + newFunctionLength));//saveZone
					newFunctionLength += 28 + instruction->Length - 4;
				}
				else {
					memCopy((*ppBuf + newFunctionLength), instruction->Opcode, instruction->Length);
					newFunctionLength += instruction->Length;
				}
		}
	}
	__finally {
		if (tabInstructions) {
			free(tabInstructions);
		}
	}

	return success;
}

BOOL GetRipRelativeAddress(_In_ INSTRUCTION_READ * instruction, _In_ void * instructionAddress, _Out_ void ** ripRelativeAddress) {
	BOOL success = TRUE;
	QWORD res = (QWORD)NULL;
	OPCODE * InstructionWithoutPrefix;

	__try {
		if (instruction == NULL || ripRelativeAddress == NULL) {
			success = FALSE;
			__leave;
		}

		InstructionWithoutPrefix = instruction->Opcode + instruction->EndOfPrefix;
		if (InstructionWithoutPrefix[0] == JMP_REL8) {
			res = GetJmpRel8Dest(InstructionWithoutPrefix, instructionAddress, instruction->Length);
		}
		else if (InstructionWithoutPrefix[0] == JMP_REL32) {
			res = GetJmpRel32Dest(InstructionWithoutPrefix, instructionAddress, instruction->Length);
		}
		else if (InstructionWithoutPrefix[0] == CALLREL32) {
			res = GetCallRel32Dest(InstructionWithoutPrefix, instructionAddress, instruction->Length);
		}
		else if (instruction->IsLoop) {
			res = GetLoopDest(InstructionWithoutPrefix, instructionAddress, instruction->Length);
		}
		else if (IsJccRel32(InstructionWithoutPrefix)) {
			res = GetJcc32Dest(InstructionWithoutPrefix, instructionAddress, instruction->Length);
		}
		else if (IsJccRel8(InstructionWithoutPrefix)) {
			res = GetJcc8Dest(InstructionWithoutPrefix, instructionAddress, instruction->Length);
		}
		//cas ModRM byte
		else if (InstructionWithoutPrefix[0] == CALL_MODRM_2 && (*(MODRM*)(InstructionWithoutPrefix + 1)).Reg == 2) {
			res = (QWORD)((BYTE*)instructionAddress + *(int*)(instruction->Opcode + instruction->ModRMPos + 1) + instruction->Length);
		}
		else if (InstructionWithoutPrefix[0] == JMP_MODRM_4 && (*(MODRM*)(InstructionWithoutPrefix + 1)).Reg == 4) {
			res = (QWORD)((BYTE*)instructionAddress + *(int*)(instruction->Opcode + instruction->ModRMPos + 1) + instruction->Length);
		}
		else {
			res = (QWORD)((BYTE*)instructionAddress + *(int*)(instruction->Opcode + instruction->ModRMPos + 1) + instruction->Length);
		}
	}
	__finally {
		if (success) {
			*ripRelativeAddress = (void*)res;
		}
	}
	return success;
}


BOOL GetNewInstructionLength(_In_ INSTRUCTION_READ * oldInstruction, _Out_ BYTE * newInstructionLength) {
	BOOL success = TRUE;
	BYTE res;
	OPCODE * instructionWithoutPrefix;

	__try {
		if (oldInstruction == NULL || newInstructionLength == NULL) {
			success = FALSE;
			__leave;
		}

		instructionWithoutPrefix = oldInstruction->Opcode + oldInstruction->EndOfPrefix;
		res = oldInstruction->EndOfPrefix;
		if (instructionWithoutPrefix[0] == JMP_REL8) {
			res += jmpUsingRetLength;
		}
		else if (instructionWithoutPrefix[0] == JMP_REL32) {
			res += jmpUsingRetLength;
		}
		else if (instructionWithoutPrefix[0] == CALLREL32) {
			res += callUsingRetLength;
		}
		else if (oldInstruction->IsLoop) {
			res += 18;
		}
		else if (IsJccRel32(instructionWithoutPrefix)) {
			res += 22;
		}
		else if (IsJccRel8(instructionWithoutPrefix)) {
			res += 18;
		}
		//cas ModRM byte
		else if (instructionWithoutPrefix[0] == CALL_MODRM_2 && (*(MODRM*)(instructionWithoutPrefix + 1)).Reg == 2) {
			res += callUsingRetLength;
		}
		else if (instructionWithoutPrefix[0] == JMP_MODRM_4 && (*(MODRM*)(instructionWithoutPrefix + 1)).Reg == 4) {
			res += jmpUsingRetLength;
		}
		else {
			//pas de += ici et c'est normal
			res = 12 + oldInstruction->Length - 4;
		}
	}
	__finally {
		if (success) {
			*newInstructionLength = res;
		}
	}
	return success;
}



BOOL CreateNewInstruction(_In_  INSTRUCTION_READ * oldINSTRUCTION, _In_ void * ripRelativeAddress, _In_ void * addressRet, _Out_ OPCODE * pBuf) {
	BOOL success = TRUE;
	int i = 0;
	OPCODE * instructionWithoutPrefix;

	__try {
		if (oldINSTRUCTION == NULL || ripRelativeAddress == NULL || pBuf == NULL) {
			success = FALSE;
			__leave;
		}

		instructionWithoutPrefix = oldINSTRUCTION->Opcode + oldINSTRUCTION->EndOfPrefix;

		if (instructionWithoutPrefix[0] == JMP_REL8) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			jmpUsingRet((pBuf + oldINSTRUCTION->EndOfPrefix), (QWORD)ripRelativeAddress);
		}
		else if (instructionWithoutPrefix[0] == JMP_REL32) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			jmpUsingRet((pBuf + oldINSTRUCTION->EndOfPrefix), (QWORD)ripRelativeAddress);
		}
		else if (instructionWithoutPrefix[0] == CALLREL32) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			callUsingRet((pBuf + oldINSTRUCTION->EndOfPrefix), (QWORD)ripRelativeAddress, (QWORD)addressRet);
		}
		else if (oldINSTRUCTION->IsLoop) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			i = oldINSTRUCTION->EndOfPrefix;
			//Si on fait le saut
			pBuf[i] += oldINSTRUCTION->Opcode[i]; i++;
			pBuf[i] = (BYTE)2; i++;
			//Si on le fait pas
			pBuf[i] = JMP_REL8; i++;
			pBuf[i] = (BYTE)14; i++;
			//Le saut
			jmpUsingRet((pBuf + i), (QWORD)ripRelativeAddress);
		}
		else if (IsJccRel32(instructionWithoutPrefix)) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			i = oldINSTRUCTION->EndOfPrefix;
			//Si on fait le saut
			pBuf[i] += oldINSTRUCTION->Opcode[i]; i++;//jmp rel32 has 2 Opcode
			pBuf[i] += oldINSTRUCTION->Opcode[i]; i++;
			*((DWORD*)(pBuf + i)) = (DWORD)2; i += 4;
			//Si on le fait pas
			pBuf[i] = JMP_REL8; i++;
			pBuf[i] = (BYTE)14; i++;
			//Le saut
			jmpUsingRet((pBuf + i), (QWORD)ripRelativeAddress);
		}
		else if (IsJccRel8(instructionWithoutPrefix)) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			i = oldINSTRUCTION->EndOfPrefix;
			//Si on fait le saut
			pBuf[i] += oldINSTRUCTION->Opcode[i]; i++;
			pBuf[i] = (BYTE)2; i++;
			//Si on le fait pas
			pBuf[i] = JMP_REL8; i++;
			pBuf[i] = (BYTE)14; i++;
			//Le saut
			jmpUsingRet((pBuf + i), (QWORD)ripRelativeAddress);
		}
		//cas ModRM byte
		else if (instructionWithoutPrefix[0] == CALL_MODRM_2 && (*(MODRM*)(instructionWithoutPrefix + 1)).Reg == 2) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			callUsingRet((pBuf + oldINSTRUCTION->EndOfPrefix), (QWORD)ripRelativeAddress, (QWORD)addressRet);
		}
		else if (instructionWithoutPrefix[0] == JMP_MODRM_4 && (*(MODRM*)(instructionWithoutPrefix + 1)).Reg == 4) {
			memCopy(pBuf, oldINSTRUCTION->Opcode, oldINSTRUCTION->EndOfPrefix);
			jmpUsingRet((pBuf + oldINSTRUCTION->EndOfPrefix), (QWORD)ripRelativeAddress);
		}
		else {
			oldINSTRUCTION->Rex.B = 0;
			if ((oldINSTRUCTION->Opcode[oldINSTRUCTION->ModRMPos] & 0x38) == 0x38) {
				pBuf[i] = PUSHRSI; i++;
				movRsiImm64((pBuf + i), (QWORD)ripRelativeAddress); i += 10;
				memCopy(pBuf + i, oldINSTRUCTION->Opcode, oldINSTRUCTION->ModRMPos); i += oldINSTRUCTION->ModRMPos;
				pBuf[i] = (oldINSTRUCTION->Opcode[oldINSTRUCTION->ModRMPos] & 0x38) + 0x06; i++;//modR/M Byte modifier pour utiliser rsi au lieu de rip + imm32
				memCopy(
					pBuf + i,
					oldINSTRUCTION->Opcode + oldINSTRUCTION->ModRMPos + 5,
					oldINSTRUCTION->Length - oldINSTRUCTION->ModRMPos - 5);
				i += oldINSTRUCTION->Length - oldINSTRUCTION->ModRMPos - 5;
				pBuf[i] = POPRSI; i++;
			}
			else {
				pBuf[i] = PUSHRDI; i++;
				movRdiImm64((pBuf + i), (QWORD)ripRelativeAddress); i += 10;
				memCopy(pBuf + i, oldINSTRUCTION->Opcode, oldINSTRUCTION->ModRMPos); i += oldINSTRUCTION->ModRMPos;

				pBuf[i] = (oldINSTRUCTION->Opcode[oldINSTRUCTION->ModRMPos] & 0x38) + 0x07; i++;//modR/M Byte modifier pour utiliser rdi au lieu de rip + imm32
				memCopy(
					pBuf + i,
					oldINSTRUCTION->Opcode + oldINSTRUCTION->ModRMPos + 5, 
					oldINSTRUCTION->Length - oldINSTRUCTION->ModRMPos - 5); 
				i += oldINSTRUCTION->Length - oldINSTRUCTION->ModRMPos - 5;
				pBuf[i] = POPRDI; i++;
			}
		}
	}
	__finally {

	}
	return success;
}

/*
	data must be before CODE (startData > startCode)
	newStartCode must have a allocated page before itself		
*/
void MovMyAssembly(OPCODE* startData, OPCODE * startCode, DWORD length, QWORD * entriesValue, DWORD entriesLength, OPCODE * newStartCode) {
	DWORD i;
	OPCODE * newStartData;

	
	newStartData = newStartCode - (startCode - startData);
	for (i = 0; i < length; i++) {
		newStartData[i] = startData[i];
	}
	for (i = 0; i < entriesLength; i++) {
		((QWORD*)newStartData)[i] = entriesValue[i];
	}
}

