#include "FlowMain.h"



/*
void addFunctionToCache(OPCODE * entryPoint, DOCKER * docker) {
	DWORD i;
	BOOL isFunctionMapped;
	ANALYSED_INSTRUCTION INSTRUCTIONs[300];
	ANALYSED_FUNCTION analyzedFunction;

	for (i = 0; i < ACCESS_ADDRESS_SIZE; i += 2) {
		if (docker->accessAddress[i + 1] == entryPoint) {
			return;
		}
	}

	for (i = 0; i < ACCESS_ADDRESS_SIZE; i += 2) {
		if (docker->accessAddress[i] >= docker->mappedFunction + docker->mappedFunctionIndex &&
			docker->accessAddress[i] <= docker->mappedFunction + docker->mappedFunctionIndex + BUFFER_Opcode_SIZE) {
			docker->accessAddress[i] = NULL;
			docker->accessAddress[i + 1] = NULL;
		}
	}

	//DebugBreak();

	analyzedFunction.Error = NoError;
	analyzedFunction.Instructions = INSTRUCTIONs;
	analyzedFunction.InstructionsMaxLength = 300;
	analyzedFunction.OriginalAddress = entryPoint;
	analyzedFunction.OriginalAddressLength = BUFFER_Opcode_SIZE;
	analyzedFunction.NewAddress = docker->mappedFunction + docker->mappedFunctionIndex;
	analyzedFunction.NewAddressLength = BUFFER_Opcode_SIZE;

	isFunctionMapped = MovEntireFunction(
		&analyzedFunction,
		docker->accessAddress,
		&docker->accessAddressIndex,
		ACCESS_ADDRESS_SIZE,
		(void(*)())docker->callbackBaseAddress,
		(void(*)())docker->codeBaseAddress,
		(QWORD *)&docker->Rip,
		&docker->saveZone);


	i = docker->accessAddressIndex;
	while (docker->accessAddress[i] >= docker->mappedFunction + docker->mappedFunctionIndex &&
		docker->accessAddress[i] <= docker->mappedFunction + docker->mappedFunctionIndex + BUFFER_Opcode_SIZE) {
		docker->accessAddress[i] = NULL;
		docker->accessAddress[i + 1] = NULL;
		i = (i + 2) % ACCESS_ADDRESS_SIZE;
	}

	if (isFunctionMapped) {
		docker->mappedFunctionIndex = (docker->mappedFunctionIndex + BUFFER_Opcode_SIZE) % (BUFFER_Opcode_SIZE * CACHE_SIZE);
	}
}*/



//extern Opcode singleOpcodeJumpConditionnelRel8[];

//first Opcode 0x0F
//extern Opcode doubleOpcodeJumpConditionnelRel32[];

#define setcallback(_callback) \
	if (_callback) {\
		placecallback(_callback, currentAddress, INSTRUCTIONcallback);\
		currentAddress += callback_SIZE;\
	}

static int machin = 0;

/*
	_In_ analyzedFunction->INSTRUCTIONsLength size of already allocated analyzedFunction->INSTRUCTIONs

	_In_ analyzedFunction->OriginalAddress address inside the function to analyse
	_Out_ analyzedFunction->OriginalAddress address of a potential start of the function (no jump after this point which go before it)

	_In_ analyzedFunction->OriginalAddressLength max authorized size for the function

	_In_ analyzedFunction->NewAddress new location for the function
	_In_ analyzedFunction->NewAddressLength max authorized size for the new function
*/
/*/
BOOL MovEntireFunction(_Inout_ ANALYSED_FUNCTION * analyzedFunction, _Out_opt_ OPCODE ** accessAddress, _Inout_opt_ DWORD * accessAddressIndex_, _In_opt_ DWORD accessAddressSize, _In_ void(*INSTRUCTIONcallback)(), _In_ void(*exitFunctioncallback)(), _In_ QWORD * newRip, _Inout_ QWORD * saveZone) {
	BOOL success = TRUE;
	DWORD accessAddressIndex;
	DWORD i;
	DWORD instructionLength, newFunctionLength;
	OPCODE * currentAddress = NULL;
	OPCODE * jmpDest;
	ANALYSED_INSTRUCTION * instruction;

	__try {

		if (!ExtractEntireFunction(analyzedFunction, TRUE, FALSE)) {
			success = FALSE;
			__leave;
		}
		instructionLength = analyzedFunction->InstructionsLength;

		if (accessAddress != NULL && accessAddressIndex_ != NULL) {
			accessAddressIndex = *accessAddressIndex_;
			accessAddress[accessAddressIndex] = analyzedFunction->NewAddress;
			accessAddress[accessAddressIndex + 1] = analyzedFunction->OriginalAddress;
			accessAddressIndex = (accessAddressIndex + 2) % accessAddressSize;
		}

		//STEP 3, create function mask
		newFunctionLength = 0;
		for (i = 0; i < instructionLength; i++) {
			instruction = analyzedFunction->Instructions + i;

			instruction->NewAddress = analyzedFunction->NewAddress + newFunctionLength;

			if ((instruction->IsJmp || instruction->IsJcc || instruction->IsLoop) && instruction->LinkedInstruction == NO_LINKED_INSTRUTION) {
				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }
				newFunctionLength += 25;
			}
			else if (instruction->IsJmp) {
				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }
				newFunctionLength += 5;
			}
			else if (instruction->IsJcc) {
				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }

				if (instruction->Opcode[instruction->EndOfPrefix] == 0xE3) {
					newFunctionLength += instruction->Length + 10;
				}
				else {
					newFunctionLength += 6;
				}
			}
			else if (instruction->IsLoop) {
				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }
				newFunctionLength += instruction->Length + 10;
			}
			else if (instruction->IsCall || instruction->IsRet) {

				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }
				newFunctionLength += 25;
			}
			else if (instruction->IsRipRelative) {
				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }
				if (instruction->Postcallback) { newFunctionLength += callback_SIZE; }
				newFunctionLength += 24 + instruction->Length - 4;
			}
			else {
				if (instruction->Precallback) { newFunctionLength += callback_SIZE; }
				if (instruction->Postcallback) { newFunctionLength += callback_SIZE; }
				newFunctionLength += instruction->Length;
			}

			if (newFunctionLength > analyzedFunction->NewAddressLength) {
				analyzedFunction->Error = NewFunctionTooBig;
				success = FALSE;
				__leave;
			}
		}


		currentAddress = analyzedFunction->NewAddress;
		for (i = 0; i < instructionLength; i++) {
			instruction = analyzedFunction->Instructions + i;

			//Last INSTRUCTION (except if linked jump)
			//Unlinked jump
			if (!(instruction->IsJmp || instruction->IsJcc || instruction->IsLoop) && i + 1 == instructionLength ||
				(instruction->IsJmp || instruction->IsJcc || instruction->IsLoop) && instruction->LinkedInstruction == NO_LINKED_INSTRUTION) {
				setcallback(instruction->Precallback);
				movQword(currentAddress, newRip, instruction->Opcode);
				jmpRel32(((QWORD)currentAddress + 20), ((QWORD)exitFunctioncallback));
				currentAddress += 25;
			}
			else if (instruction->IsJmp) {
				jmpDest = analyzedFunction->Instructions[instruction->LinkedInstruction].NewAddress;

				jmpRel32(currentAddress, jmpDest);
				currentAddress += 5;
			}
			else if (instruction->IsJcc) {
				jmpDest = analyzedFunction->Instructions[instruction->LinkedInstruction].NewAddress;

				setcallback(instruction->Precallback);
				if (instruction->Opcode[instruction->EndOfPrefix] == 0xE3) {
					loopRel32(currentAddress, jmpDest, instruction->Opcode, instruction->Length);
					currentAddress += instruction->Length + 10;
				}
				else if (instruction->Opcode[instruction->EndOfPrefix] == 0x0F) {
					jccRel32(currentAddress, jmpDest, instruction->Opcode[instruction->EndOfPrefix + 1]);
					currentAddress += 6;
				}
				else {
					jccRel32(currentAddress, jmpDest, instruction->Opcode[instruction->EndOfPrefix] + 0x10);
					currentAddress += 6;
				}

			}
			else if (instruction->IsLoop) {
				jmpDest = analyzedFunction->Instructions[instruction->LinkedInstruction].NewAddress;

				setcallback(instruction->Precallback);
				loopRel32(currentAddress, jmpDest, instruction->Opcode, instruction->Length);
				currentAddress += instruction->Length + 10;
			}
			else if (instruction->IsCall || instruction->IsRet) {

				setcallback(instruction->Precallback);
				if (instruction->IsCall && accessAddress != NULL) {//To catch return
					accessAddress[accessAddressIndex] = currentAddress + 25;
					accessAddress[accessAddressIndex + 1] = instruction->Opcode + instruction->Length;
					accessAddressIndex = (accessAddressIndex + 2) % accessAddressSize;
				}

				movQword(currentAddress, newRip, instruction->Opcode);
				jmpRel32(((QWORD)currentAddress + 20), ((QWORD)exitFunctioncallback));
				currentAddress += 25;
			}
			else if (instruction->IsRipRelative) {
				setcallback(instruction->Precallback);
				RewriteRipRelativeInstruction(&(instruction->Instruction), currentAddress, saveZone);
				currentAddress += 24 + instruction->Length - 4;
				setcallback(instruction->Postcallback);
			}
			else {
				setcallback(instruction->Precallback);
				memCopy(currentAddress, instruction->Opcode, instruction->Length);
				currentAddress += instruction->Length;
				setcallback(instruction->Postcallback);
			}
		}

	}
	__finally {
		if (success) {
			if (accessAddressIndex_ != NULL) {
				*accessAddressIndex_ = accessAddressIndex;
				FlushInstructionCache(GetCurrentProcess(), analyzedFunction->NewAddress, currentAddress - analyzedFunction->NewAddress);
			}
		}
	}

	return success;
}/**/


#undef setcallback
