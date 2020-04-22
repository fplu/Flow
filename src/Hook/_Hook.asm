
extern saveRetAddress
extern getRetAddress
extern getCallAddress
extern getPrecallback
extern getPostcallback
extern getOnlyPrecallback
extern getRegister

global __fonctionHookGeneric


section .text ; makes this executable
	USE64

%macro CALL1ARG 2; function,arg
	mov rcx, %2
	call %1
%endmacro


struc CONTEXT 

    ;//
    ;// Register parameter home addresses.
    ;//
    ;// N.B. These fields are for convience - they could be used to extend the
    ;//      context record in the future.
    ;//

    .P1Home: resq 1;
    .P2Home: resq 1;
    .P3Home: resq 1;
    .P4Home: resq 1;
    .P5Home: resq 1;
    .P6Home: resq 1;

    ;//
    ;// Control flags.
    ;//

    .ContextFlags: resd 1;
    .MxCsr: resd 1;

    ;//
    ;// Segment Registers and processor flags.
    ;//

    .SegCs: resw 1;
    .SegDs: resw 1;
    .SegEs: resw 1;
    .SegFs: resw 1;
    .SegGs: resw 1;
    .SegSs: resw 1;
    .EFlags: resd 1;

    ;//
    ;// Debug registers
    ;//

    .Dr0: resq 1;
    .Dr1: resq 1;
    .Dr2: resq 1;
    .Dr3: resq 1;
	.Dr6: resq 1;
    .Dr7: resq 1;

    ;//
    ;// Integer registers.
    ;//

    .Rax: resq 1;
    .Rcx: resq 1;
    .Rdx: resq 1;
    .Rbx: resq 1;
    .Rsp: resq 1;
    .Rbp: resq 1;
    .Rsi: resq 1;
    .Rdi: resq 1;
    .R8: resq 1;
    .R9: resq 1;
    .R10: resq 1;
    .R11: resq 1;
    .R12: resq 1;
    .R13: resq 1;
    .R14: resq 1;
    .R15: resq 1;

    ;//
    ;// Program counter.
    ;//

    .Rip: resq 1;

    ;//
    ;// Floating point state.
    ;//

  
            .Header0: resq 4;
			.Legacy: resq 16;
            .Xmm0: resq 2;
            .Xmm1: resq 2;
            .Xmm2: resq 2;
            .Xmm3: resq 2;
            .Xmm4: resq 2;
            .Xmm5: resq 2;
            .Xmm6: resq 2;
            .Xmm7: resq 2;
            .Xmm8: resq 2;
            .Xmm9: resq 2;
            .Xmm10: resq 2;
            .Xmm11: resq 2;
            .Xmm12: resq 2;
            .Xmm13: resq 2;
            .Xmm14: resq 2;
            .Xmm15: resq 2;
   

    ;//
    ;// Vector registers.
    ;//

	.VectorRegister: resq 52;
    .VectorControl: resq 1;

    ;//
    ;// Special debug control registers.
    ;//

    .DebugControl: resq 1;
    .LastBranchToRip: resq 1;
    .LastBranchFromRip: resq 1;
    .LastExceptionToRip: resq 1;
    .LastExceptionFromRip: resq 1;
endstruc

	%macro PUSH_REGISTER_ENTIER_VOLATILE 0
		pushfq
		push rax
		push rcx
		push rdx
		push r8
		push r9
		push r10
		push r11
	%endmacro

	%macro PUSH_REGISTER_ENTIER_CALLEE_SAFE 0
		push rdx
		push rbp
		push rsi
		push rdi
		push r12
		push r13
		push r14
		push r15
	%endmacro
	%macro PUSH_REGISTER_XMM_VOLATILE 0
		sub rsp, 60h
		movdqu [rsp], xmm5
		movdqu [rsp + 10h], xmm4
		movdqu [rsp + 20h], xmm3
		movdqu [rsp + 30h], xmm2
		movdqu [rsp + 40h], xmm1
		movdqu [rsp + 50h], xmm0
	%endmacro
	%macro POP_REGISTER_ENTIER_CALLEE_SAFE 0
		pop r15
		pop r14
		pop r13
		pop r12
		pop rdi
		pop rsi
		pop rbp
		pop rdx
	%endmacro
	%macro POP_REGISTER_ENTIER_VOLATILE 0
		pop r11
		pop r10
		pop r9
		pop r8
		pop rdx
		pop rcx
		pop rax
		popfq
	%endmacro
	%macro POP_REGISTER_XMM_VOLATILE 0
		movdqu  xmm5, [rsp]
		movdqu  xmm4, [rsp + 10h]
		movdqu  xmm3, [rsp + 20h]
		movdqu  xmm2, [rsp + 30h]
		movdqu  xmm1, [rsp + 40h]
		movdqu  xmm0, [rsp + 50h]
		add rsp, 60h
	%endmacro

%macro SAVEREGISTERMACRO 0
	push rbp
	push rbp
	mov rbp, rsp
	
	PUSH_REGISTER_ENTIER_VOLATILE
	PUSH_REGISTER_ENTIER_CALLEE_SAFE
	PUSH_REGISTER_XMM_VOLATILE

	sub rsp, 100h
	call getRegister
	add rsp, 100h
	mov [rbp], rax
	
	POP_REGISTER_XMM_VOLATILE
	POP_REGISTER_ENTIER_CALLEE_SAFE
	POP_REGISTER_ENTIER_VOLATILE


	mov rsp, [rbp]
	lea rsp, [rsp + CONTEXT.EFlags + 8]
	pushfq
	lea rsp, [rsp - CONTEXT.EFlags ]

	lea rbp, [rbp + 10h]
	mov [rsp + CONTEXT.Rsp], rbp; it's normal
	mov rbp, [rbp - 8]

	mov [rsp + CONTEXT.Rax], rax
	mov [rsp + CONTEXT.Rcx], rcx
	mov [rsp + CONTEXT.Rdx], rdx
	mov [rsp + CONTEXT.Rbx], rbx
	mov [rsp + CONTEXT.Rbp], rbp
	mov [rsp + CONTEXT.Rsi], rsi
	mov [rsp + CONTEXT.Rdi], rdi
	mov [rsp + CONTEXT.R8], r8

	mov [rsp + CONTEXT.R9], r9
	mov [rsp + CONTEXT.R10], r10
	mov [rsp + CONTEXT.R11], r11
	mov [rsp + CONTEXT.R12], r12
	mov [rsp + CONTEXT.R13], r13
	mov [rsp + CONTEXT.R14], r14
	mov [rsp + CONTEXT.R15], r15 

	movdqu [rsp + CONTEXT.Xmm0], xmm0
	movdqu [rsp + CONTEXT.Xmm1], xmm1
	movdqu [rsp + CONTEXT.Xmm2], xmm2
	movdqu [rsp + CONTEXT.Xmm3], xmm3
	movdqu [rsp + CONTEXT.Xmm4], xmm4
	movdqu [rsp + CONTEXT.Xmm5], xmm5
	movdqu [rsp + CONTEXT.Xmm6], xmm6
	movdqu [rsp + CONTEXT.Xmm7], xmm7
	movdqu [rsp + CONTEXT.Xmm8], xmm8
	movdqu [rsp + CONTEXT.Xmm9], xmm9
	movdqu [rsp + CONTEXT.Xmm10], xmm10
	movdqu [rsp + CONTEXT.Xmm11], xmm11
	movdqu [rsp + CONTEXT.Xmm12], xmm12
	movdqu [rsp + CONTEXT.Xmm13], xmm13
	movdqu [rsp + CONTEXT.Xmm14], xmm14
	movdqu [rsp + CONTEXT.Xmm15], xmm15


	mov [rsp + CONTEXT.SegCs], cs
	mov [rsp + CONTEXT.SegDs], ds
	mov [rsp + CONTEXT.SegEs], es
	mov [rsp + CONTEXT.SegFs], fs
	mov [rsp + CONTEXT.SegGs], gs
	mov [rsp + CONTEXT.SegSs], ss


	mov rsp, [rsp + CONTEXT.Rsp]

%endmacro


%macro GETREGISTERMACRO 0
	sub rsp, 100h
	call getRegister
	add rsp, 100h

	lea rsp, [rax + CONTEXT.EFlags]
	popfq

	mov rcx, [rax + CONTEXT.Rcx]
	mov rdx, [rax + CONTEXT.Rdx]
	mov rbx, [rax + CONTEXT.Rbx]
	mov rsp, [rax + CONTEXT.Rsp]
	mov rbp, [rax + CONTEXT.Rbp]
	mov rsi, [rax + CONTEXT.Rsi]
	mov rdi, [rax + CONTEXT.Rdi]
	mov r8, [rax + CONTEXT.R8]
	mov r9, [rax + CONTEXT.R9]
	mov r10, [rax + CONTEXT.R10]
	mov r11, [rax + CONTEXT.R11]
	mov r12, [rax + CONTEXT.R12]
	mov r13, [rax + CONTEXT.R13]
	mov r14, [rax + CONTEXT.R14]
	mov r15, [rax + CONTEXT.R15] 

	movdqu xmm0, [rax + CONTEXT.Xmm0]
	movdqu xmm1, [rax + CONTEXT.Xmm1]
	movdqu xmm2, [rax + CONTEXT.Xmm2]
	movdqu xmm3, [rax + CONTEXT.Xmm3]
	movdqu xmm4, [rax + CONTEXT.Xmm4]
	movdqu xmm5, [rax + CONTEXT.Xmm5]
	movdqu xmm6, [rax + CONTEXT.Xmm6]
	movdqu xmm7, [rax + CONTEXT.Xmm7]
	movdqu xmm8, [rax + CONTEXT.Xmm8]
	movdqu xmm9, [rax + CONTEXT.Xmm9]
	movdqu xmm10, [rax + CONTEXT.Xmm10]
	movdqu xmm11, [rax + CONTEXT.Xmm11]
	movdqu xmm12, [rax + CONTEXT.Xmm12]
	movdqu xmm13, [rax + CONTEXT.Xmm13]
	movdqu xmm14, [rax + CONTEXT.Xmm14]
	movdqu xmm15, [rax + CONTEXT.Xmm15]

;	mov cs, [rax + CONTEXT.SegCs]
	mov ds, [rax + CONTEXT.SegDs]
	mov es, [rax + CONTEXT.SegEs]
	mov fs, [rax + CONTEXT.SegFs]
	mov gs, [rax + CONTEXT.SegGs]
	mov ss, [rax + CONTEXT.SegSs]

	mov rax, [rax + CONTEXT.Rax]

%endmacro



	__fonctionHookGeneric:

		SAVEREGISTERMACRO

		mov rbp, rsp
						;008h	Sert pour l'Address du jump effectue avec un ret ret
		sub rsp, 118h	;108h	Sert pour protéger la stack
						;008h	Sert pour l'Address de retour
						;008h	Sert pour l'Address du jump effectue avec un ret ret
		and rsp, 0FFFFFFFFFFFFFFF0h

		;START manage Ret
			call getPostcallback
			test rax, rax
			jz dontChangeRet
				;On sauvegarde l'Address de retour
				CALL1ARG saveRetAddress,[rbp]
		
				;On met la nouvelle Address de retour du call
				mov rcx, AddressRetHookTrampoline
				mov qword [rbp], rcx
			dontChangeRet:
		;END manage Ret

		


		;START pre callback
			call getPrecallback
			test rax, rax
			jz ignorePrecallback
	
				sub rsp, 8; for alignement
				PUSH_REGISTER_XMM_VOLATILE
					lea rcx, [rel finPrecallback]
					push rcx
					push rax
					ret
				POP_REGISTER_XMM_VOLATILE

			finPrecallback:
			ignorePrecallback:
		;END pre callback

		;START manage Call
			;On push l'Address du call
			call getOnlyPrecallback
			test rax, rax
			jnz dontGetOriginalCallAddress
				call getCallAddress
			dontGetOriginalCallAddress:

			mov qword [rbp - 8h], rax
		;END manage Call

		lea rsp, [rbp - 8]

			GETREGISTERMACRO
			lea rsp, [rsp - 8]
			ret
			AddressRetHookTrampoline:
			SAVEREGISTERMACRO

		mov rbp, rsp
		sub rsp, 108h
		and rsp, 0FFFFFFFFFFFFFFF0h


		;START post callback
			call getPostcallback
			test rax, rax
			jz ignorePostcallback
	
				sub rsp, 8; for alignement
				PUSH_REGISTER_XMM_VOLATILE
					lea rcx, [rel finPostcallback]
					push rcx
					push rax
					ret
				POP_REGISTER_XMM_VOLATILE

			finPostcallback:
			ignorePostcallback:
		;END post callback

		mov rsp, rbp


		sub rsp, 100h
		call getRetAddress
		add rsp, 100h
		push rax
		GETREGISTERMACRO
		

		ret



