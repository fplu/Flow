;EXTERN g_mmm
;EXTERN g_ppmp

global EndGoCallback
global StartGoCallback
global StartCodeGoCallback
global StartCodeGoCallback_2
global StartCodeGoCallback_Ret
global StartCodeGoCallback_Jmp
global StartCodeGoCallback_Call
global StartCodeGoCallback_Syscall
global StartCodeGoCallback_CallShadowStack
global StartCodeGoCallback_RetShadowStack

global MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_1
global MOV_RSI_RDI_DISPLACEMENT_DOCKER_TRACE_LOOP_TRACE_INSTRUCTION_1
global MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_1
global MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_2
global MOV_RSI_RDI_DISPLACEMENT_DOCKER_CALLBACK_2
global MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_2 

global SAVE_RSI
global SAVE_RDI
global MOV_RSI_TLS_DOCKER
global XCHG_RSI_TLS_TMP
global LOAD_RSI
global LOAD_RDI
global JMP_TLS_TMP

global SAVE_RSI_2
global SAVE_RDI_2
global MOV_RSI_TLS_DOCKER_2
global XCHG_RSI_TLS_TMP_2
global LOAD_RSI_2
global LOAD_RDI_2
global JMP_TLS_TMP_2

global MOV_RAX_TO_TLS1_3
global MOV_TLSPAGE_TO_RAX_3_1
global MOV_RAX_TO_TLS2_3_1
global MOV_TLS2_TO_RCX_3
global MOV_RAX_TO_TLS2_3_2
global MOV_TLSPAGE_TO_RAX_3_2
global MOV_TLS1_TO_RAX_3
global JMP_AT_TLS2_3

global MOV_RAX_TO_TLS1_4
global MOV_TLSPAGE_TO_RAX_4
global MOV_TLS2_TO_RCX_4
global MOV_RAX_TO_TLS2_4

global MOV_RAX_TO_TLS1_5
global MOV_TLSPAGE_TO_RAX_5_1
global MOV_TLS2_TO_RCX_5
global MOV_TLSPAGE_TO_RDX_5
global MOV_RAX_TO_TLS2_5
global MOV_TLSPAGE_TO_RAX_5_2
global MOV_TLS3_TO_RCX_5
global MOV_TLS1_TO_RAX_5
global JMP_AT_TLS2_5

global MOV_RAX_TO_TLS1_6
global MOV_TLSPAGE_TO_RAX_6_1
global MOV_TLS1_TO_RAX_6_1
global MOV_RAX_TO_TLS2_6
global MOV_TLSPAGE_TO_RAX_6_2
global MOV_TLS1_TO_RAX_6_2
global JMP_AT_TLS2_6

global MOV_RAX_TO_TLS1_7
global MOV_TLSPAGE_TO_RAX_7_1
global MOV_RCX_TO_TLS3_7
global MOV_TLSPAGE_TO_RAX_7_2
global MOV_TLS1_TO_RAX_7
global JMP_AT_TLS3_7

global MOV_RAX_TO_TLS1_8
global MOV_TLSPAGE_TO_RAX_8_1
global MOV_TLS2_TO_RCX_8
global MOV_RAX_TO_TLS3_8
global MOV_TLSPAGE_TO_RAX_8_2
global MOV_TLS1_TO_RAX_8
global JMP_AT_TLS3_8


%macro  readCache 1 
	mov     r8, rcx
;	test    edx, edx
;	je      saut1_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x1a (00007ff9`fac947ba)
;	nop     word [rax+rax]
;	mov		rax, [rel G_MMM]
;	mov     eax, dword [rax + 0x10];[INSTRUCTIONPointerManagement!g_mmm+0x10 (00007ff9`facaff50)]
;	saut2_%1:
;	test    eax, eax
;	jne     saut2_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x10 (00007ff9`fac947b0)
;	saut1_%1:
	mov     rax, r8
	shr     rax, 24h
	cmp     rax, 7FFh
	jne     saut3_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x63 (00007ff9`fac94803)
	mov		rax, [rel G_PPMP]
	mov     rax, qword [rax];INSTRUCTIONPointerManagement!g_ppmp (00007ff9`facaff68)]
	shr     rcx, 0Ch
	and     ecx, 0FFFFFFh
	mov     r9, qword [rax+rcx*8]
	test    r9, r9
	je      saut3_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x63 (00007ff9`fac94803)
	sub     r8, qword [r9+8]
	mov     rax, qword [r9+10h]
	mov     ecx, dword [rax+r8*4]
	cmp     ecx, 0FFFFFFFFh
	jne     saut4_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x59 (00007ff9`fac947f9)
	xor     edx, edx
	mov     eax, edx
	jmp fin_%1
	saut4_%1:
	mov     rdx, rcx
	add     rdx, qword [r9]
	mov     rax, rdx
	jmp fin_%1

	saut3_%1:
	mov [rsp + 20h], r10
	mov [rsp + 28h], r11


	mov		r11, [rel G_MMM]
	mov     r11d, dword [r11 + 0x08];[INSTRUCTIONPointerManagement!g_mmm+0x8 (00007ff9`facaff48)]
	xor     edx, edx
	mov     r9d, edx
	test    r11d, r11d
	je      saut5_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0xa2 (00007ff9`fac94842)
	mov     rcx, qword [rel G_MMM];INSTRUCTIONPointerManagement!g_mmm (00007ff9`facaff40)]
	mov     rcx, qword [rcx];INSTRUCTIONPointerManagement!g_mmm (00007ff9`facaff40)]
	add     rcx, 1Ch
	nop     
	saut8_%1:
	mov     r10, qword [rcx-14h]
	cmp     r10, r8
	ja      saut6_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x93 (00007ff9`fac94833)
	mov     eax, dword [rcx]
	add     rax, r10
	cmp     rax, r8
	ja      saut7_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0xa5 (00007ff9`fac94845)
	saut6_%1:
	inc     r9d
	add     rcx, 68h; WARNING
	cmp     r9d, r11d
	jb      saut8_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0x80 (00007ff9`fac94820)
	saut5_%1:
	xor     eax, eax
	jmp fin2_%1
	saut7_%1: 
	lfence  
	mov     eax, r9d
	imul    rcx, rax, 68h; WARNING 
	mov		rax, [rel G_MMM]
	add     rcx, qword [rax];[INSTRUCTIONPointerManagement!g_mmm (00007ff9`facaff40)]
	sub     r8, qword [rcx+8]
	mov     rax, qword [rcx+10h]
	mov     r9d, dword [rax+r8*4]
	cmp     r9d, 0FFFFFFFFh
	je      saut9_%1;	INSTRUCTIONPointerManagement!GetNewAddressFromOldAddress+0xd1 (00007ff9`fac94871)
	mov     edx, r9d
	add     rdx, qword [rcx]
	saut9_%1:
	mov     rax, rdx

	fin2_%1:
	mov r10, [rsp + 20h]
	mov r11, [rsp + 28h]

	fin_%1:
%endmacro


section .text exec write ; makes this executable
	USE64

align 8
StartGoCallback:
		MappedInstructions : dq 0
		MappedInstructionsMinimal : dq 0
		G_MMM : dq 0
		G_PPMP : dq 0
		g_SyscallCallback : dq 0
		g_SyscallCallbackLength : dq 0
		callInfo : dq 0

StartCodeGoCallback:;exitCallback
	;	db 0cch;

		SAVE_RSI:
			db 0,0,0,0,0,0,0,0,0 ; save rsi
		SAVE_RDI:
			db 0,0,0,0,0,0,0,0,0 ; save rdi


		MOV_RSI_TLS_DOCKER:
			db 0,0,0,0,0,0,0,0,0		; mov rsi tls docker
		mov rdi, rsi

		;mov docker->analyzedInstruction, mappedModule->analyzedInstruction	
			mov rsi, [rel MappedInstructions]
			MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_1 :
				db 0, 0, 0, 0, 0, 0, 0
			;mov [rdi + 5F8h], rsi 
		;mov docker->analyzedInstruction, mappedModule->analyzedInstruction

		;mov addressJump, docker->traceLoopTraceInstruction + 20
			MOV_RSI_RDI_DISPLACEMENT_DOCKER_TRACE_LOOP_TRACE_INSTRUCTION_1 :
				db 0, 0, 0, 0, 0, 0, 0
			;mov rsi, [rdi + 05E0h]		;rsi is jump destination
			lea rsi, [rsi + 20]			;rsi += 20
		;mov addressJump, docker->traceLoopTraceInstruction	+ 20

		;mov docker->instructionId, mappedModule->Index
			XCHG_RSI_TLS_TMP:
				db 0,0,0,0,0,0,0,0,0	;xchg rsi, tls index
			mov esi, esi
			MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_1 :
				db 0, 0, 0, 0, 0, 0, 0
			;mov [rdi + 5F0h], rsi
			;mov dword[rdi + 5F0h + 4], 0
		;mov docker->instructionId, mappedModule->Index


		LOAD_RSI:
			db 0,0,0,0,0,0,0,0,0	; load rsi
		LOAD_RDI:
			db 0,0,0,0,0,0,0,0,0	; load rdi

		JMP_TLS_TMP:
			db 0,0,0,0,0,0,0,0		; jmp tls

StartCodeGoCallback_2:
	;	db 0cch;

		SAVE_RSI_2:
			db 0,0,0,0,0,0,0,0,0 ; save rsi
		SAVE_RDI_2:
			db 0,0,0,0,0,0,0,0,0 ; save rdi


		MOV_RSI_TLS_DOCKER_2:
			db 0,0,0,0,0,0,0,0,0		; mov rsi tls docker
		mov rdi, rsi

		;mov docker->analyzedInstruction, mappedModule->analyzedInstruction	
			mov rsi, [rel MappedInstructionsMinimal]
			MOV_RDI_DISPLACEMENT_DOCKER_ANALYSED_INSTRUCTION_RSI_2 :
				db 0, 0, 0, 0, 0, 0, 0
			;mov [rdi + 5F8h], rsi 
		;mov docker->analyzedInstruction, mappedModule->analyzedInstruction

		;mov addressJump, docker->callback
			MOV_RSI_RDI_DISPLACEMENT_DOCKER_CALLBACK_2 :
				db 0, 0, 0, 0, 0, 0, 0
			;mov rsi, [rdi + 0600h]		;rsi is jump destination
		;mov addressJump, docker->callback	

		;mov docker->instructionId, mappedModule->Index
			XCHG_RSI_TLS_TMP_2:
				db 0,0,0,0,0,0,0,0,0	;xchg rsi, tls index
			mov esi, esi
			MOV_RDI_DISPLACEMENT_DOCKER_INSTRUCTION_ID_RSI_2 :
				db 0, 0, 0, 0, 0, 0, 0
			;mov [rdi + 5F0h], rsi
			;mov dword[rdi + 5F0h + 4], 0
		;mov docker->instructionId, mappedModule->Index


		LOAD_RSI_2:
			db 0,0,0,0,0,0,0,0,0	; load rsi
		LOAD_RDI_2:
			db 0,0,0,0,0,0,0,0,0	; load rdi

		JMP_TLS_TMP_2:
			db 0,0,0,0,0,0,0,0		; jmp tls




StartCodeGoCallback_Ret:
	

	MOV_RAX_TO_TLS1_3:
		db 0,0,0,0,0,0,0,0,0
	MOV_TLSPAGE_TO_RAX_3_1:
		db 0,0,0,0,0,0,0,0,0
	mov [rax + 00h], rcx
	mov [rax + 08h], rdx
	mov [rax + 10h], r8
	mov [rax + 18h], r9
;	mov [rax + 20h], r10
;	mov [rax + 28h], r11
	mov [rax + 30h], rsp

	;db 0CCh

	mov rcx, [rsp];get ret address from rsp
	mov rdx, 1;sync

	lea rsp, [rax + 40h]
	pushfq

	mov rsp, rax

StartFastCacheAccess:


;	mov rax, 0
;	jmp fin

readCache 1
	test rax, rax
	jnz finfin
		;db 0CCh

		MOV_RAX_TO_TLS2_3_1:;clear TLS 2 because rax is zero here
			db 0,0,0,0,0,0,0,0,0
		lea rax, [rel StartCodeGoCallback];lea rax, [rel exit]
	finfin:

	
	MOV_TLS2_TO_RCX_3:
		db 0,0,0,0,0,0,0,0,0
	mov ecx, ecx

	MOV_RAX_TO_TLS2_3_2:
		db 0,0,0,0,0,0,0,0,0


	MOV_TLSPAGE_TO_RAX_3_2:
		db 0,0,0,0,0,0,0,0,0

	lea rsp, [rax + 38h]
	popfq


	mov rsp, [rax + 30h]
	lea rsp, [rsp + rcx]

	;db 0CCh

	mov rcx, [rax + 00h]
	mov rdx, [rax + 08h]
	mov r8, [rax + 10h]
	mov r9, [rax + 18h]
;	mov r10, [rax + 20h]
;	mov r11, [rax + 28h]

	MOV_TLS1_TO_RAX_3:
		db 0,0,0,0,0,0,0,0,0


	JMP_AT_TLS2_3:
		db 0,0,0,0,0,0,0,0


StartCodeGoCallback_Jmp:
		
	MOV_RAX_TO_TLS1_4:
		db 0,0,0,0,0,0,0,0,0
	MOV_TLSPAGE_TO_RAX_4:
		db 0,0,0,0,0,0,0,0,0
	mov [rax + 00h], rcx
	mov [rax + 08h], rdx
	mov [rax + 10h], r8
	mov [rax + 18h], r9
;	mov [rax + 20h], r10
;	mov [rax + 28h], r11
	mov [rax + 30h], rsp

	lea rsp, [rax + 40h]
	pushfq

	mov rsp, rax

	MOV_TLS2_TO_RCX_4:
		db 0,0,0,0,0,0,0,0,0
	mov rdx, 1;sync
	xor eax, eax
	MOV_RAX_TO_TLS2_4:
		db 0,0,0,0,0,0,0,0,0

	
	jmp StartFastCacheAccess

StartCodeGoCallback_Call:
	
	;	db 0CCh
	jmp ghu_5
	MOV_RAX_TO_TLS1_5:
		db 0,0,0,0,0,0,0,0,0
	MOV_TLSPAGE_TO_RAX_5_1:
		db 0,0,0,0,0,0,0,0,0
	ghu_5:
	mov [rax + 00h], rcx
	mov [rax + 08h], rdx
	mov [rax + 10h], r8
	mov [rax + 18h], r9
	;mov [rax + 20h], r10
	;mov [rax + 28h], r11
	mov [rax + 30h], rsp

	lea rsp, [rax + 40h]
	pushfq

	mov rsp, rax

	mov rdx, 1;sync
	MOV_TLS2_TO_RCX_5:
		db 0,0,0,0,0,0,0,0,0

	readCache 5

	;db 0cch

	MOV_TLSPAGE_TO_RDX_5:
		db 0,0,0,0,0,0,0,0,0
	
	test rax, rax
	jnz finfin_5
		
		lea rsp, [rdx + 38h]
		popfq

		mov rsp, [rdx + 30h]
		lea rax, [rel StartCodeGoCallback];lea rax, [rel exit]

	jmp finfinfin_5
	finfin_5:

		lea rsp, [rdx + 38h]
		popfq


		mov [rdx + 1000h - 28h], rax;[CallDest];(store at rax + 1000h - 28h)
		mov rax, rdx
		
		mov rsp, [rax + 30h]
		mov rcx, [rax + 00h]
		mov rdx, [rax + 08h]
		mov r8, [rax + 10h]
		mov r9, [rax + 18h]
		jmp StartCodeGoCallback_CallShadowStack

		MOV_TLS3_TO_RCX_5:
			db 0,0,0,0,0,0,0,0,0
		push rcx

	finfinfin_5:

	MOV_RAX_TO_TLS2_5:
		db 0,0,0,0,0,0,0,0,0


	MOV_TLSPAGE_TO_RAX_5_2:
		db 0,0,0,0,0,0,0,0,0

	mov rcx, [rax + 00h]
	mov rdx, [rax + 08h]
	mov r8, [rax + 10h]
	mov r9, [rax + 18h]
;	mov r10, [rax + 20h]
;	mov r11, [rax + 28h]

	MOV_TLS1_TO_RAX_5:
		db 0,0,0,0,0,0,0,0,0

	JMP_AT_TLS2_5:
		db 0,0,0,0,0,0,0,0

StartCodeGoCallback_Syscall:

	MOV_RAX_TO_TLS1_6:
		db 0,0,0,0,0,0,0,0,0
	MOV_TLSPAGE_TO_RAX_6_1:
		db 0,0,0,0,0,0,0,0,0
	mov [rax + 00h], rcx
	mov [rax + 08h], rdx
	mov [rax + 30h], rsp
	lea rsp, [rax + 40h]
	pushfq

	mov rcx, [rel g_SyscallCallback]
	mov rdx, [rel g_SyscallCallbackLength]
	MOV_TLS1_TO_RAX_6_1:
		db 0,0,0,0,0,0,0,0,0
	mov eax, eax


	loop_6:
		mov esp, [rcx + 8]
		cmp esp, eax
		je bad_end_6
		lea rcx, [rcx + 16]
		dec rdx
		test rdx, rdx
	jnz loop_6
	
		;happy end		
	jmp fin_6
	bad_end_6:
		lea rax, [rel StartCodeGoCallback];lea rax, [rel exit]
		MOV_RAX_TO_TLS2_6:
			db 0,0,0,0,0,0,0,0,0
	fin_6:

	MOV_TLSPAGE_TO_RAX_6_2:
		db 0,0,0,0,0,0,0,0,0
	lea rsp, [rax + 38h]
	popfq
	mov rcx, [rax + 00h]
	mov rdx, [rax + 08h]
	mov rsp, [rax + 30h]
	MOV_TLS1_TO_RAX_6_2:
		db 0,0,0,0,0,0,0,0,0

	JMP_AT_TLS2_6:
		db 0,0,0,0,0,0,0,0

StartCodeGoCallback_CallShadowStack:


	jmp ghu_7
	MOV_RAX_TO_TLS1_7:
		db 0,0,0,0,0,0,0,0,0
	MOV_TLSPAGE_TO_RAX_7_1:
		db 0,0,0,0,0,0,0,0,0
	ghu_7:

	mov [rax + 00h], rcx
	mov [rax + 08h], rdx

	mov rcx, [rax + 1000h - 28h];[CallDest];(store at rax + 1000h - 28h)
	MOV_RCX_TO_TLS3_7:
		db 0,0,0,0,0,0,0,0,0

	mov rdx, [rax + 1000h - 20h];[CallIndex];(store at rax + 1000h - 20h)
	lea rdx, [rdx * 2 + rdx]

	mov rcx, [rax + 1000h - 08h];[tlsShadowStackCounter]; (store at rax + 1000h - 08h)
	lea rcx, [rcx + 1]
	movzx rcx, cl
	mov [rax + 1000h - 08h], rcx;[tlsShadowStackCounter] (store at rax + 1000h - 08h)
	lea rcx, [rcx * 2 + rcx]
	mov rax, [rax + 1000h - 10h];tlsShadowStack (store at rax + 1000h - 10h)
	lea rcx, [rcx * 8 + rax]
	mov rax, [rel callInfo];
	lea rdx, [rax + rdx * 8]
	mov rax, [rdx]
	mov [rcx], rax; old return value
	push rax
	mov rax, [rdx + 08h]
	mov [rcx + 08h], rax; new return value
	mov rax, [rdx + 10h]
	mov [rcx + 10h], rax; optional callback

	MOV_TLSPAGE_TO_RAX_7_2:
		db 0,0,0,0,0,0,0,0,0
	mov rcx, [rax + 00h]
	mov rdx, [rax + 08h]
	MOV_TLS1_TO_RAX_7:
		db 0,0,0,0,0,0,0,0,0

	JMP_AT_TLS3_7:
		db 0,0,0,0,0,0,0,0

StartCodeGoCallback_RetShadowStack:

	MOV_RAX_TO_TLS1_8:
		db 0,0,0,0,0,0,0,0,0
	MOV_TLSPAGE_TO_RAX_8_1:
		db 0,0,0,0,0,0,0,0,0
	mov [rax + 00h], rcx
	mov [rax + 08h], rsp
	lea rsp, [rax + 18h]
	pushfq

	;lea rsp, [rel jmpHere_1_8]

	Mov rcx, [rax + 1000h - 08h]; [tlsShadowStackCounter]; (store at rax + 1000h - 08h)
	
	mov rsp, rcx
	dec rsp
	movzx rsp, spl
	mov [rax + 1000h - 08h], rsp; [tlsShadowStackCounter](store at rax + 1000h - 08h)

	lea rcx, [rcx * 2 + rcx]
	mov rsp, [rax + 08h]
	mov rax, [rax + 1000h - 10h];tlsShadowStack (store at rax + 1000h - 10h)
	lea rcx, [rcx * 8 + rax]
	mov rsp, [rsp];
	;
	;void * oldAddress
	;void * newAddress
	;void * callback
	;
	cmp [rcx], rsp
	jne the_bad_way_8

		mov rax, [rcx + 10h]			;store callback address in rax
		test rax, rax					;if(callback != null) {
		jz avoid_callback_8				;
			db 0CCh						;	__debugbreak()
			jmp StartCodeGoCallback_2	;	performcallback
		avoid_callback_8:				;}

		mov rax, [rcx + 8h]
		MOV_TLS2_TO_RCX_8:
			db 0,0,0,0,0,0,0,0,0
		mov ecx, ecx
		;lea rax, [rel StartCodeGoCallback_Ret]; WARNING A SUPPRIMER DANS LE FUTURE LORSQUE LA CALLSTACK SERA NETTOYEE (OU PARTIELLEMENT INVALIDE)
		;xor ecx, ecx
		jmp end_of_the_good_way_8

	the_bad_way_8:
		lea rax, [rel StartCodeGoCallback_Ret]
		xor ecx, ecx
	end_of_the_bad_way_8:
	end_of_the_good_way_8:

	MOV_RAX_TO_TLS3_8:
		db 0,0,0,0,0,0,0,0,0

	MOV_TLSPAGE_TO_RAX_8_2:
		db 0,0,0,0,0,0,0,0,0
	lea rsp, [rax + 10h]
	popfq
	mov rsp, [rax + 08h]
	lea rsp, [rsp + rcx]
	mov rcx, [rax + 00h]
	MOV_TLS1_TO_RAX_8:
		db 0,0,0,0,0,0,0,0,0

	JMP_AT_TLS3_8:
		db 0,0,0,0,0,0,0,0


EndGoCallback:


