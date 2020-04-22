#ifndef _Opcode_H_
#define _Opcode_H_

typedef unsigned char OPCODE;

#define RAX_ID 0x0
#define RCX_ID 0x1
#define RDX_ID 0x2
#define RBX_ID 0x3
#define RSP_ID 0x4
#define RBP_ID 0x5
#define RSI_ID 0x6
#define RDI_ID 0x7
#define R8_ID 0x0
#define R9_ID 0x1
#define R10_ID 0x2
#define R11_ID 0x3
#define R12_ID 0x4
#define R13_ID 0x5
#define R14_ID 0x6
#define R15_ID 0x7

#define ES_ID 0x0
#define CS_ID 0x1
#define SS_ID 0x2
#define DS_ID 0x3
#define FS_ID 0x4
#define GS_ID 0x5

#define XMM0_ID 0x0
#define XMM1_ID 0x1
#define XMM2_ID 0x2
#define XMM3_ID 0x3
#define XMM4_ID 0x4
#define XMM5_ID 0x5
#define XMM6_ID 0x6
#define XMM7_ID 0x7
#define XMM8_ID 0x0
#define XMM9_ID 0x1
#define XMM10_ID 0x2
#define XMM11_ID 0x3
#define XMM12_ID 0x4
#define XMM13_ID 0x5
#define XMM14_ID 0x6
#define XMM15_ID 0x7


#define SEGMENT_OVERRIDE_ES_PREFIX 0x26
#define SEGMENT_OVERRIDE_CS_PREFIX 0x2E
#define SEGMENT_OVERRIDE_SS_PREFIX 0x36
#define SEGMENT_OVERRIDE_DS_PREFIX 0x3E
#define SEGMENT_OVERRIDE_FS_PREFIX 0x64
#define SEGMENT_OVERRIDE_GS_PREFIX 0x65
#define OPERAND_SIZE_PREFIX 0x66
#define ADDRESS_SIZE_PREFIX 0x67
#define LOCK_PREFIX	0xF0
#define REPE_PREFIX 0xF3
#define REPNE_PREFIX 0xF2
#define REX_W 0x08
#define REX_R 0x04
#define REX_X 0x02
#define REX_B 0x01



#define JMPREL32 0xE9
#define JMPREL8 0xEB

#define CALLREL32 0xE8

#define LOOPREL8 0xE2
#define LOOPEREL8 0xE1
#define LOOPNEREL8 0xE0

#define NOP 0x90

#define PUSHIMM32 0x68

#define PUSHRSI 0x56
#define PUSHRDI 0x57

#define POPRSI 0x5E
#define POPRDI 0x5F

#define RET_NEAR 0xC3
#define RET_FAR 0xCB
#define RET_NEAR_POP_IMM6 0xC2
#define RET_FAR_POP_IMM16 0xCA

#define MOV_RDI_IMM64 0xBF48
#define MOV_RSI_IMM64 0xBE48
#define MOV_RSP_IMM64 0xBC48
#define MOV_RCX_IMM64 0xB948
#define MOV_RAX_IMM64 0xB848

#define MAX_INSTRUCTION_SIZE 16

#define JMP_REL_32_SIZE 5

#define READ_SEG 0x8C
#define WRITE_SEG 0x8E

#define IS_MODRM_RIP_RELATIVE(modrm) \
 (((modrm) & 0xC7) == 0x05)


//7 Opcodes length
#define movRdiImm32DisplacementRsi(addressOpcode, imm32) \
	* (BYTE*)(addressOpcode + 0) = 0x48;\
	* (BYTE*)(addressOpcode + 1) = 0x89;\
	* (BYTE*)(addressOpcode + 2) = 0xB7;\
	* (DWORD*)(addressOpcode + 3) = (imm32);

#define movRsiRdiImm32Displacement(addressOpcode, imm32) \
	* (BYTE*)(addressOpcode + 0) = 0x48;\
	* (BYTE*)(addressOpcode + 1) = 0x8B;\
	* (BYTE*)(addressOpcode + 2) = 0xB7;\
	* (DWORD*)(addressOpcode + 3) = (imm32);


//10 Opcodes length
#define movAtRaxRelImm32(addressOpcode, rel, imm32)\
	(*(WORD*)(addressOpcode) = (0x80C7),\
	* (DWORD*)(addressOpcode + 2) = (DWORD)(rel),\
	* (DWORD*)(addressOpcode + 6) = (DWORD)(imm32),\
	10)

//20 Opcodes length
#define movAtRaxRelImm64(addressOpcode, rel, imm64)\
	(movAtRaxRelImm32(addressOpcode, rel, imm64),\
	movAtRaxRelImm32(addressOpcode + 10, rel + 4, ((QWORD)imm64) >> 32),\
	20)

//10 Opcodes length
#define movRegImm64(addressOpcode, regId, imm64) \
	*(WORD*)(addressOpcode) = (0xB848 + (regId * 0x100)); \
	*((QWORD*)(addressOpcode + 2)) = imm64;//imm64

//10 Opcodes length
#define movRegExtendImm64(addressOpcode, regId, imm64) \
	*(WORD*)(addressOpcode) = (0xB84D + (regId * 0x100)); \
	*((QWORD*)(addressOpcode + 2)) = imm64;//imm64

//10 Opcodes length
#define movRdiImm64(addressOpcode, imm64) \
	*(WORD*)(addressOpcode) = MOV_RDI_IMM64; \
	*((QWORD*)(addressOpcode + 2)) = imm64;//imm64

//10 Opcodes length
#define movRsiImm64(addressOpcode, imm64) \
	*(WORD*)(addressOpcode) = MOV_RSI_IMM64; \
	*((QWORD*)(addressOpcode + 2)) = imm64;//imm64

//10 Opcodes length
#define movRcxImm64(adresseOpcode, imm64) \
	*(WORD*)(adresseOpcode) = MOV_RCX_IMM64; \
	*((QWORD*)(adresseOpcode + 2)) = imm64;//imm64

//10 Opcodes length
#define movRaxImm64(adresseOpcode, imm64) \
	*(WORD*)(adresseOpcode) = MOV_RAX_IMM64; \
	*((QWORD*)(adresseOpcode + 2)) = imm64;//imm64


//10 Opcodes length
#define movRspImm64(adresseOpcode, imm64) \
	*(WORD*)(adresseOpcode) = MOV_RSP_IMM64; \
	*((QWORD*)(adresseOpcode + 2)) = imm64;//imm64

//mov qword ptr[rsp + 4], imm32 	
//8 Opcodes length
#define pushHighImm32(addressOpcode, imm32) \
	*(BYTE*)(addressOpcode) = 0xC7; \
	*(BYTE*)(addressOpcode + 1) = 0x44; \
	*(BYTE*)(addressOpcode + 2) = 0x24; \
	*(BYTE*)(addressOpcode + 3) = 0x04; \
	*(DWORD*)(addressOpcode + 4) = (DWORD)(imm32);

//8 Opcodes length
#define jmpAtTls(addressOpcode, tlsId) \
	((BYTE*)addressOpcode)[0] = 0x65;\
	((BYTE*)addressOpcode)[1] = 0xFF;\
	((BYTE*)addressOpcode)[2] = 0x24;\
	((BYTE*)addressOpcode)[3] = 0x25;\
	*(DWORD*)(((BYTE*)addressOpcode) + 4) = (DWORD)(0x1480 + (tlsId) * 8);

/*
2 go range
6 Opcodes length
*/
#define jmpAtAddress(addressOpcode, address) \
	((BYTE*)(addressOpcode))[0] = 0xFF;\
	((BYTE*)(addressOpcode))[1] = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 2) = (DWORD)(((QWORD)(address)) - ((QWORD)(addressOpcode)) - (QWORD)6);


//14 Opcodes length
#define jmpImm64(addressOpcode, imm64) \
	((BYTE*)(addressOpcode))[0] = 0xFF;\
	((BYTE*)(addressOpcode))[1] = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 2) = 0x0;\
	*(QWORD*)(((BYTE*)(addressOpcode)) + 6) = (QWORD)(imm64);

//13 Opcodes length
#define pushimm64(addressOpcode, imm64) \
	*(BYTE*)(addressOpcode) = PUSHIMM32; \
	*(DWORD*)(addressOpcode + 1) = (DWORD)(imm64); \
	pushHighImm32((addressOpcode + 5), (((QWORD)(imm64)) >> 32)) \

//14 Opcodes length
#define jmpUsingRet(addressOpcode, addressJump) \
	pushimm64(addressOpcode, addressJump); \
	*(BYTE*)(addressOpcode + 13) = RET

//27 Opcodes length
#define callUsingRet(addressOpcode, addressJump, addressRet) \
	pushimm64(addressOpcode, addressRet); \
	pushimm64(addressOpcode + 13, addressJump); \
	*(BYTE*)(addressOpcode + 26) = RET


#if 1
//5 Opcodes length (be careful, 2Go range)
#define jmpRel32(addressOpcode, addressDest) \
	*(PBYTE)(addressOpcode) = (UCHAR)0xE9; \
	*(DWORD*)(((ULONG_PTR)(addressOpcode)) + (ULONG_PTR)1) = (DWORD)(((ULONG_PTR)(addressDest)-(ULONG_PTR)(((ULONG_PTR)(addressOpcode)) + (ULONG_PTR)5)) & MAXDWORD);
#else
//5 Opcodes length (be careful, 2Go range)
#define jmpRel32(addressOpcode, addressDest) \
	*(BYTE*)(addressOpcode) =  0xE9; \
	*(DWORD*)(((QWORD)addressOpcode) + 1) = -(QWORD)(((QWORD)addressOpcode) + 5) + (QWORD)(addressDest);
#endif

//6 Opcodes length (be careful, 2Go range)
#define jccRel32(addressOpcode, addressDest, jccOpcode) \
	*(BYTE*)(addressOpcode) =  0x0F; \
	((BYTE*)(addressOpcode))[1] =  jccOpcode; \
	*(DWORD*)(((QWORD)addressOpcode) + 2) = (DWORD)((QWORD)(addressDest) - (QWORD)(((QWORD)addressOpcode) + 6));

//10 Opcodes length (be careful, 2Go range), register and stack 100% safe
#define movDword(addressOpcode, addressDest, value)\
	((BYTE*)addressOpcode)[0] = 0xC7;\
	((BYTE*)addressOpcode)[1] = 0x05;\
	*(DWORD*)(((BYTE*)addressOpcode) + 2) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 10)));\
	*(DWORD*)(((BYTE*)addressOpcode) + 6) = (DWORD)(QWORD)(value);

//20 Opcodes length (be careful, 2Go range), register and stack 100% safe
#define movQword(addressOpcode, addressDest, value)\
	movDword(addressOpcode, addressDest, value);\
	movDword(((BYTE*)addressOpcode + 10), addressDest + 4, (((QWORD)value) >> 32));
/*	((BYTE*)addressOpcode)[0] = 0xC7;\
	((BYTE*)addressOpcode)[1] = 0x05;\
	*(DWORD*)(((BYTE*)addressOpcode) + 2) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 10)));\
	*(DWORD*)(((BYTE*)addressOpcode) + 6) = (DWORD)(QWORD)(value);\
	((BYTE*)addressOpcode)[10] = 0xC7;\
	((BYTE*)addressOpcode)[11] = 0x05;\
	*(DWORD*)(((BYTE*)addressOpcode) + 12) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 20)) + 4);\
	*(DWORD*)(((BYTE*)addressOpcode) + 16) = (DWORD)(((QWORD)value) >> 32);*/


//7 Opcodes length (be careful, 2Go range) stack 100% safe
#define movQwordRegister(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x48; \
	((BYTE*)addressOpcode)[1] = 0x89; \
	((BYTE*)addressOpcode)[2] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 3) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 7)));

//7 Opcodes length (be careful, 2Go range) stack 100% safe
#define movQwordRegisterR8_R15(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x4C; \
	((BYTE*)addressOpcode)[1] = 0x89; \
	((BYTE*)addressOpcode)[2] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 3) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 7)));

//7 Opcodes length (be careful, 2Go range) stack 100% safe
#define movRegisterQword(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x48; \
	((BYTE*)addressOpcode)[1] = 0x8B; \
	((BYTE*)addressOpcode)[2] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 3) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 7)));

//7 Opcodes length (be careful, 2Go range) stack 100% safe
#define movRegisterQwordR8_R15(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x4C; \
	((BYTE*)addressOpcode)[1] = 0x8B; \
	((BYTE*)addressOpcode)[2] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 3) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 7)));


#define callOneArg(adresseOpcode, arg1, functionCalled) \
	movRcxImm64(adresseOpcode, arg1);\
	callUsingRet((adresseOpcode + 10), functionCalled, (QWORD)(adresseOpcode + 37));

#define subRsp100h(adresseOpcode) \
	((BYTE*)(adresseOpcode))[0] = 0x48; \
	((BYTE*)(adresseOpcode))[1] = 0x81; \
	((BYTE*)(adresseOpcode))[2] = 0xEC; \
	((BYTE*)(adresseOpcode))[3] = 0x00; \
	((BYTE*)(adresseOpcode))[4] = 0x01; \
	((BYTE*)(adresseOpcode))[5] = 0x00; \
	((BYTE*)(adresseOpcode))[6] = 0x00;

#define addRsp100h(adresseOpcode) \
	((BYTE*)(adresseOpcode))[0] = 0x48; \
	((BYTE*)(adresseOpcode))[1] = 0x81; \
	((BYTE*)(adresseOpcode))[2] = 0xC4; \
	((BYTE*)(adresseOpcode))[3] = 0x00; \
	((BYTE*)(adresseOpcode))[4] = 0x01; \
	((BYTE*)(adresseOpcode))[5] = 0x00; \
	((BYTE*)(adresseOpcode))[6] = 0x00;

//8 opcodes length
#define decDwordTls(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x250CFF65;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 4) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define addRaxTls(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x04034865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//6 opcodes length
#define cmpRaxImm32(addressOpcode, imm32)\
	*(WORD*)(((BYTE*)(addressOpcode)) + 0) = 0x3D48;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 2) = (imm32);

//24 opcodes length
#define movQwordToTls(addressOpcode, tlsId, value)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x2504c765;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 4) = (0x1480 + (tlsId) * 8 + 4);\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 8) = (((QWORD)value) >> 32);\
	movDwordToTls((((BYTE*)(addressOpcode)) + 12), tlsId, value)

//12 opcodes length
#define movDwordToTls(addressOpcode, tlsId, value)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x2504c765;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 4) = (0x1480 + (tlsId) * 8);\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 8) = ((DWORD)(QWORD)value);

//9 opcodes length
#define xchgRsiToTls(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x34874865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movRsiToTls(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x34894865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movRdiToTls(addressOpcode, tlsId)\
	(*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x3C894865,\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25,\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8),\
	 9);

//9 opcodes length
#define movRcxToTls(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x0C894865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movRaxToTls(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x04894865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movTlsToRsi(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x348b4865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movTlsToRdi(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x3C8b4865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movTlsToRcx(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x0C8b4865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movTlsToRdx(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x148b4865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//9 opcodes length
#define movTlsToRax(addressOpcode, tlsId)\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 0) = 0x048b4865;\
	*(BYTE*)(((BYTE*)(addressOpcode)) + 4) = 0x25;\
	*(DWORD*)(((BYTE*)(addressOpcode)) + 5) = (0x1480 + (tlsId) * 8);

//8 opcodes length
#define movdqaXmm0_7ToMem(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x66; \
	((BYTE*)addressOpcode)[1] = 0x0F; \
	((BYTE*)addressOpcode)[2] = 0x7F; \
	((BYTE*)addressOpcode)[3] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 4) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 8)));

//9 opcodes length
#define movdqaXmm8_15ToMem(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x66; \
	((BYTE*)addressOpcode)[1] = 0x48; \
	((BYTE*)addressOpcode)[2] = 0x0F; \
	((BYTE*)addressOpcode)[3] = 0x7F; \
	((BYTE*)addressOpcode)[4] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 5) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 9)));

//8 opcodes length
#define movdqaMemToXmm0_7(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x66; \
	((BYTE*)addressOpcode)[1] = 0x0F; \
	((BYTE*)addressOpcode)[2] = 0x6F; \
	((BYTE*)addressOpcode)[3] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 4) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 8)));

//9 opcodes length
#define movdqaMemToXmm8_15(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = 0x66; \
	((BYTE*)addressOpcode)[1] = 0x48; \
	((BYTE*)addressOpcode)[2] = 0x0F; \
	((BYTE*)addressOpcode)[3] = 0x6F; \
	((BYTE*)addressOpcode)[4] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 5) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 9)));


//6 opcodes length
#define movMemEqualSeg(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = READ_SEG; \
	((BYTE*)addressOpcode)[1] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 2) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 6)));


//6 opcodes length
#define movSegEqualMem(addressOpcode, addressDest, registerId)\
	((BYTE*)addressOpcode)[0] = WRITE_SEG; \
	((BYTE*)addressOpcode)[1] = (0x05 | (BYTE)((registerId) << 3)); \
	*(DWORD*)(((BYTE*)addressOpcode) + 2) = (DWORD)(((QWORD)addressDest) - ((QWORD)(((BYTE*)addressOpcode) + 6)));


#define jmpUsingRetLength 14
#define callUsingRetLength 27




#endif // !_MACRO_H
